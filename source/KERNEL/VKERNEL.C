/*
	Program	vkernel.c
	Date	2002/7/6 .. 2002/8/15
	Copyright (C) 2002 by AKIYA

	カーネル本体をここで記述

	　main() を含み、ここでカーネルを初期化する。 main() が実行される前に、
	stack_area をスタック領域の先頭とし、そのサイズが先頭に格納してあるこ
	と。現在のスタックがこの領域の最後端から使用され、割当てサイズ内にある
	こと。また、main_tcb が VT_TCB として確保され、 ctsk 領域が定義されて
	いる必要がある。（main_tcb は、そのままカーネルが使用する。）

	　タスクＩＤは、タスクコントロールブロックへのポインタで置き換えられて
	いる。（kernel.c）

	　カーネル処理および割込みハンドラ等は、現在実行中のコンテキストのス
	タックを使用して処理が行われる。（μＩＴＲＯＮ４．０仕様と異なる）
	よって、 exd_tsk を実装することはできない。（実現するには、別途サービ
	スコール実施タスク等が必要。）
*/

#include <limits.h>
#include "include\system.h"

/* ローカルプロトタイプ */
static void _kernel_dispatcher( void );
static void _kernel_task_entry( void );
static void _kernel_link_priority( VT_TCB * );
static void _kernel_link_out_priority( VT_TCB * );
static void _kernel_wait_refresh( void );
static VT_TCB *_kernel_srch_priority_run( void );
static PRI _kernel_srch_priority_exist( VT_TCB * );
static void _kernel_initialize_stack( void );
static char *_kernel_get_stack( long, int );
static void _kernel_free_stack( char *, long );

/* カーネルステート */
volatile VT_KST _kernel_state = {0,		/* ディスパッチカウンタ＝０ */
				FALSE,		/* ディスパッチ要求なし */
				FALSE,		/* ディスパッチ禁止 */
				FALSE,		/* ＣＰＵアンロック */
				0		/* システム時刻更新カウンタ＝０ */
				};

/* 実行中タスク＆プライオリティリンク */
static volatile VT_TCB *_kernel_nxt_task = NULL;
static volatile VT_TCB *_kernel_run_task = NULL;
static volatile VT_TCB *_kernel_pri_link[TMAX_TPRI];

/* 割り込みハンドラコールテーブル */
extern FP _kernel_inthdl_table[];

/* メインタスクのＴＣＢを参照 */
extern VT_TCB main_tcb;

/* カーネルの始動 */
void main( void )
{
	vloc_cpu();
	{
		int i;

		for ( i = 0; i < TMAX_TPRI; i++ )
			_kernel_pri_link[i] = NULL;
	}
	_kernel_initialize_stack();	/* スタックヒープ初期化 */
	if ( main_tcb.ctsk.stk == NULL ) {
		/* 現在のスタックを割り当て */
		if ( (main_tcb.ctsk.stk = (VP)_kernel_get_stack( main_tcb.ctsk.stksz, 1 )) == NULL )
			return;		/* error ! */
	}
	main_tcb.rtsk.tskstat = TTS_RUN;
	main_tcb.rtsk.tskpri = main_tcb.ctsk.itskpri;
	main_tcb.rtsk.tskbpri = main_tcb.ctsk.itskpri;
	main_tcb.rtsk.tskwait = 0;
	main_tcb.rtsk.wobjid = 0;
	main_tcb.rtsk.lefttmo = 0;
	main_tcb.rtsk.actcnt = 0;
	main_tcb.rtsk.wupcnt = 0;
	main_tcb.rtsk.suscnt = 0;
	_kernel_link_priority( &main_tcb );
	_kernel_run_task = &main_tcb;
	ini_tmr();
	vunl_cpu();
	(_kernel_run_task->ctsk.task)(_kernel_run_task->ctsk.exinf);
	vext_tsk();
}

/* --- タスク管理機能 --- */

/* タスクの生成 *********************/
/* vt_tcb == NULL で acre_tsk 処理  */
/* 属性、パラメータエラーには未対応 */
ER vcre_tsk( VT_TCB *vt_tcb, T_CTSK *pk_ctsk )
{
	BOOL cpu_locked = _kernel_state.cpuloc;
	ER ercd = E_OK;

	if (!cpu_locked)	/* cre_tsk では、FALSE のはずであるが、 */
		vloc_cpu();	/* vext_tsk 等からも呼ばれる為、必要 */
	if ( vt_tcb == NULL ) {
		if ( (vt_tcb = (VT_TCB *)_kernel_get_stack( sizeof(VT_TCB), 0 )) == NULL )
			ercd = E_NOID;
		else
			ercd = P2ID(vt_tcb);
	} else if ( _kernel_srch_priority_exist( vt_tcb ) != 0 )
			ercd = E_OBJ;
	if ( (ercd == E_OK) || (ercd > 0) ) {
		vt_tcb->ctsk.tskatr = pk_ctsk->tskatr;
		vt_tcb->ctsk.exinf = pk_ctsk->exinf;
		vt_tcb->ctsk.task = pk_ctsk->task;
		vt_tcb->ctsk.itskpri = pk_ctsk->itskpri;
		vt_tcb->ctsk.stksz = pk_ctsk->stksz;
		if ( (vt_tcb->ctsk.stk = pk_ctsk->stk) == NULL )
			if ( (vt_tcb->ctsk.stk = (VP)_kernel_get_stack( pk_ctsk->stksz, 1 )) == NULL ) {
				if ( ercd > 0 )
					_kernel_free_stack( ID2P(ercd), sizeof(VT_TCB) );
				ercd = E_NOMEM;
			}
	}
	if ( (ercd == E_OK) || (ercd > 0) ) {
		vt_tcb->rtsk.tskstat = TTS_DMT;
		vt_tcb->rtsk.tskpri = pk_ctsk->itskpri;
		vt_tcb->rtsk.tskbpri = pk_ctsk->itskpri;
		vt_tcb->rtsk.tskwait = 0;
		vt_tcb->rtsk.wobjid = 0;
		vt_tcb->rtsk.lefttmo = 0;
		vt_tcb->rtsk.actcnt = 0;
		vt_tcb->rtsk.wupcnt = 0;
		vt_tcb->rtsk.suscnt = 0;
		_kernel_link_priority( vt_tcb );
		if ( pk_ctsk->tskatr & TA_ACT )
			vact_tsk( vt_tcb );
	}
	if (!cpu_locked)
		vunl_cpu();
	return ercd;
}

/* タスクの起動 **********/
/* E_ID を返すことはない */
ER vact_tsk( VT_TCB *vt_tcb )
{
	BOOL cpu_locked = _kernel_state.cpuloc;
	ER ercd = E_OK;

	if (!cpu_locked)	/* act_tsk では、FALSE のはずであるが、 */
		vloc_cpu();	/* vext_tsk 等からも呼ばれる為、必要 */
	if ( vt_tcb == TSK_SELF )	/* TSK_SELF == 0 */
		vt_tcb = _kernel_run_task;
	if ( _kernel_srch_priority_exist( vt_tcb ) == 0 )
		ercd = E_NOEXS;
	else {
		if ( vt_tcb->rtsk.tskstat == TTS_DMT ) {
			vt_tcb->rtsk.tskstat = TTS_RDY;
			_kernel_set_adr( vt_tcb, (long)_kernel_task_entry );
			_kernel_set_stk( vt_tcb, (long)vt_tcb->ctsk.stk+vt_tcb->ctsk.stksz );
			_kernel_state.dspreq = TRUE;
		} else {
			if ( vt_tcb->rtsk.actcnt >= TMAX_ACTCNT )
				ercd = E_QOVR;
			else
				vt_tcb->rtsk.actcnt++;
		}
	}
	if (!cpu_locked)
		vunl_cpu();
	return ercd;
}

/* 自タスクの終了 *****/
/* 資源開放等は未実装 */
void vext_tsk( void )
{
	int act;

	vloc_cpu();
	if ( _kernel_state.dspdis == TRUE )	/* エラー終了がない為、 */
		vena_dsp();			/* ディスパッチを有効！ */
	/*
	ここで、セマフォ等を開放（現在、未実装）
	*/
	_kernel_state.dspreq = TRUE;
	if ( act = _kernel_run_task->rtsk.actcnt ) {
		_kernel_link_out_priority( _kernel_run_task );
		vcre_tsk( _kernel_run_task, &(_kernel_run_task->ctsk) );
		vact_tsk( _kernel_run_task );
		_kernel_run_task->rtsk.actcnt = act-1;
		_kernel_state.dspcnt++;
		_kernel_load_context( _kernel_run_task, 1 );	/* longjmp */
	} else {
		_kernel_run_task->rtsk.tskstat = TTS_DMT;
		vunl_cpu();
	}
}

/* --- タスク付属同期機能 --- */

/* 自タスクの遅延 */
ER vdly_tsk( RELTIM dlytim )
{
	ER ercd = E_OK;

	while ( dlytim > (INT_MAX-1) ) {
		if ( (ercd = vdly_tsk(INT_MAX-1)) != E_OK )
			return ercd;
		else
			dlytim -= (INT_MAX-1) +1;
	}
	vloc_cpu();
	_kernel_link_out_priority( _kernel_run_task );
	_kernel_link_priority( _kernel_run_task );
	_kernel_run_task->rtsk.tskstat = TTS_WAI;
	_kernel_run_task->rtsk.lefttmo = dlytim +1;
	_kernel_run_task->rtsk.tskwait = TTW_DLY;
	_kernel_state.dspreq = TRUE;
	vunl_cpu();
	if ( _kernel_run_task->rtsk.lefttmo != 0 )
		ercd = E_RLWAI;
	_kernel_run_task->rtsk.tskwait = 0;
	return ercd;
}

/* --- 時間管理機能 --- */

/* タイムティックの供給 */
ER ivsig_tim( void )
{
	BOOL cpu_locked = _kernel_state.cpuloc;

	if (!cpu_locked)
		vloc_cpu();
	if ( ++_kernel_state.timreq == 0 )
		_kernel_state.timreq--;	/*(オーバーフローなら戻す)*/
	else
		_kernel_state.dspreq = TRUE;
	if (!cpu_locked)
		vunl_cpu();
	return E_OK;
}

/* --- システム状態管理機能 --- */

/* ＣＰＵロック状態への移行 */
ER vloc_cpu( void )
{
	_kernel_int_disable();			/* 割り込み禁止 */
	_kernel_state.cpuloc = TRUE;
	return E_OK;
}

/* ＣＰＵロック状態の解除 */
ER vunl_cpu( void )
{
	if ( _kernel_state.dspcnt == 0 )
		if ( _kernel_state.dspreq == TRUE )
			_kernel_dispatcher();
	_kernel_state.cpuloc = FALSE;
	_kernel_int_enable();			/* 割り込み許可 */
	return E_OK;
}

/* ディスパッチの禁止 */
ER vdis_dsp( void )
{
	_kernel_state.dspdis = TRUE;
	return E_OK;
}

/* ディスパッチの許可 */
ER vena_dsp( void )
{
	_kernel_state.dspdis = FALSE;
	if ( _kernel_state.cpuloc == FALSE ) {
		vloc_cpu();
		vunl_cpu();	/* ここで、ディスパッチャが起動するかも */
	}
	return E_OK;
}

/* --- 割り込み管理機能 --- */

/* 割込みハンドラの定義 */
ER vdef_inh( INHNO inhno, T_DINH *pk_dinh )
{
	if ( pk_dinh == NULL )
		_kernel_inthdl_table[VCT2INO(inhno)] = (FP)_kernel_inthdl_null;
	else
		_kernel_inthdl_table[VCT2INO(inhno)] = pk_dinh->inthdr;
	return E_OK;
}

/* --- カーネル処理 --- */

/* ディスパッチャー（ＣＰＵロック状態で呼び出すこと） */
/* ディスパッチに先立って、時間管理処理をここで実行。 */
static void _kernel_dispatcher( void )
{
	_kernel_state.dspcnt++;		/* 非タスクコンテキストの開始 */
	vunl_cpu();
	while ( _kernel_state.dspreq == TRUE ) {
		_kernel_state.dspreq = FALSE;
		while ( _kernel_state.timreq > 0 ) {
			vloc_cpu();
			_kernel_state.timreq--;
			_kernel_wait_refresh();
			vunl_cpu();
			/*
			ここで、タイマーイベント
			*/
		}
		if ( _kernel_state.dspdis == FALSE ) {
			vloc_cpu();
			if ( (_kernel_nxt_task = _kernel_srch_priority_run()) != NULL ) {
				if ( _kernel_run_task != _kernel_nxt_task )
					if ( _kernel_save_context( &(_kernel_run_task->ctxbuf) ) == 0 ) {
						if ( _kernel_run_task->rtsk.tskstat == TTS_RUN )
							_kernel_run_task->rtsk.tskstat = TTS_RDY;
						_kernel_run_task = _kernel_nxt_task;
						_kernel_run_task->rtsk.tskstat = TTS_RUN;
						_kernel_load_context( &(_kernel_run_task->ctxbuf), 1 );
					}
			} else
				_kernel_state.dspreq = TRUE;
			vunl_cpu();
		}
	}
	vloc_cpu();
	_kernel_state.dspcnt--;		/* 非タスクコンテキストの終了 */
}

/* タスク開始エントリー */
static void _kernel_task_entry( void )
{
	_kernel_state.dspcnt--;	/* 最初はディスパッチャの途中でここへ来る */
	vunl_cpu();		/* ここで、ディスパッチリクエストを消化！ */
	(_kernel_run_task->ctsk.task)(_kernel_run_task->ctsk.task);
	vext_tsk();
}

/* 割り込み入出処理 */
void _kernel_inthdl_io_service( INHNO intno )
{
	vloc_cpu();
	_kernel_state.dspcnt++;
	(_kernel_inthdl_table[VCT2INO(intno)])();/* ＣＰＵロック状態でハンドラへ */
	vloc_cpu();
	if ( --_kernel_state.dspcnt == 0 )
		_kernel_dispatcher();
}

/* 未登録割り込み */
void _kernel_inthdl_null( void )
{
}

/* プライオリティリンク */
static void _kernel_link_priority( VT_TCB *tcb )
{
	PRI pri;

	pri = tcb->rtsk.tskpri;
	if ( _kernel_pri_link[pri-1] == NULL ) {
		tcb->lnk.fwd = tcb;
		tcb->lnk.bak = tcb;
		_kernel_pri_link[pri-1] = tcb;
	} else {
		tcb->lnk.fwd = _kernel_pri_link[pri-1];
		tcb->lnk.bak = (_kernel_pri_link[pri-1])->lnk.bak;
		((_kernel_pri_link[pri-1])->lnk.bak)->lnk.fwd = tcb;
		(_kernel_pri_link[pri-1])->lnk.bak = tcb;
	}
}

/* プライオリティリンク削除 */
static void _kernel_link_out_priority( VT_TCB *tcb )
{
	PRI pri;

	pri = tcb->rtsk.tskpri;
	if ( _kernel_pri_link[pri-1] == tcb )
		if ( tcb->lnk.fwd == tcb ) {
			_kernel_pri_link[pri-1] = NULL;
			return;
		} else
			_kernel_pri_link[pri-1] = tcb->lnk.fwd;
	(tcb->lnk.fwd)->lnk.bak = tcb->lnk.bak;
	(tcb->lnk.bak)->lnk.fwd = tcb->lnk.fwd;
}

/* ウエイトカウンタの更新 */
static void _kernel_wait_refresh( void )
{
	VT_TCB *tcb;
	int i;

	for ( i = 0; i < TMAX_TPRI; i++ ) {
		if ( (tcb = _kernel_pri_link[i]) != NULL ) {
			do {
				if ( tcb->rtsk.tskstat == TTS_WAI ) {
					if ( tcb->rtsk.lefttmo > 0 ) {
						if ( --(tcb->rtsk.lefttmo) == 0 )
							tcb->rtsk.tskstat = TTS_RDY;
					}
				} else if ( tcb->rtsk.tskstat == TTS_WAS ) {
					if ( tcb->rtsk.lefttmo > 0 ) {
						if ( --(tcb->rtsk.lefttmo) == 0 )
							tcb->rtsk.tskstat = TTS_SUS;
					}
				}
				tcb = tcb->lnk.fwd;
			} while (tcb != _kernel_pri_link[i] );
		}
	}
}

/* 実行すべきタスクを得る */
static VT_TCB *_kernel_srch_priority_run( void )
{
	VT_TCB *tcb;
	int i;

	for ( i = 0; i < TMAX_TPRI; i++ ) {
		if ( (tcb = _kernel_pri_link[i]) != NULL ) {
			do {
				if ( (tcb->rtsk.tskstat == TTS_RUN) || (tcb->rtsk.tskstat == TTS_RDY) )
					return tcb;
				tcb = tcb->lnk.fwd;
			} while (tcb != _kernel_pri_link[i] );
		}
	}
	return NULL;
}

/* プライオリティリンク検索 */
static PRI _kernel_srch_priority_exist( VT_TCB *vt_tcb )
{
	VT_TCB *tcb;
	PRI i;

	for ( i = 0; i < TMAX_TPRI; i++ ) {
		if ( (tcb = _kernel_pri_link[i]) != NULL ) {
			do {
				if ( tcb == vt_tcb )
					return i+1;
				tcb = tcb->lnk.fwd;
			} while (tcb != _kernel_pri_link[i] );
		}
	}
	return 0;
}

/* --- スタック管理 --- */

/* 未使用領域管理構造体 */
/* 未使用領域をサイズと次のブロックへのポインタで管理する */
typedef struct stk_t {
	long		size;
	struct stk_t	*next;
}STK_t;

/* スタックエリア参照 */
extern char stack_area[];

/* スタック管理機構初期化 *****************/
static void _kernel_initialize_stack( void )
{
	STK_t *stk;

	stk = (STK_t *)stack_area;
	stk->next = (STK_t *)((long)stk+sizeof(STK_t));
	(stk->next)->size = stk->size - sizeof(STK_t);
	(stk->next)->next = NULL;
	stk->size = sizeof(STK_t) -1;/* 領域の先頭が割り当てられないように細工 */
}

/* スタック管理機構からメモリを取得 ****************/
/* mode=0:下位側,=1:上位側アドレスを優先して割当て */
static char *_kernel_get_stack( long size, int mode )
{
	STK_t *fptr;
	STK_t *sptr;
	STK_t *wptr;

	size = ((size+3)/4)*4;
	if ( size < sizeof(STK_t) )
		size = sizeof(STK_t);
	for ( fptr = sptr = NULL, wptr = (STK_t *)stack_area; wptr->next != NULL; wptr = wptr->next )
		if ( (wptr->next)->size == size ) {
			fptr = wptr;
			if ( mode == 0 )
				break;
		} else if ( (wptr->next)->size >= size+sizeof(STK_t) ) {
			if ( (sptr == NULL) || (mode == 1) )
				sptr = wptr;
		}
	if ( (fptr == NULL) && (sptr != NULL) ) {
		if ( mode == 0 ) {
			fptr = sptr;
			sptr = sptr->next;
			wptr = (STK_t *)((long)sptr + size);
			wptr->next = sptr->next;
			wptr->size = sptr->size - size;
			sptr->size = size;
			sptr->next = wptr;
		} else {
			fptr = sptr->next;
			fptr->size = fptr->size - size;
			wptr = (STK_t *)((long)fptr + fptr->size);
			wptr->size = size;
			wptr->next = fptr->next;
			fptr->next = wptr;
		}
	}
	if ( fptr != NULL ) {
		wptr = fptr;
		fptr = fptr->next;
		wptr->next = fptr->next;
	}
	return (char *)fptr;
}

/* スタック管理機構へメモリを開放 */
static void _kernel_free_stack( char *topadr, long size )
{
	STK_t *wptr;

	size = ((size+3)/4)*4;
	if ( size < sizeof(STK_t) )
		size = sizeof(STK_t);
	for ( wptr = (STK_t *)stack_area; wptr->next != NULL; wptr = wptr->next )
		if ( (long)(wptr->next) > (long)topadr )
			break;
	((STK_t *)topadr)->size = size;
	((STK_t *)topadr)->next = wptr->next;
	wptr->next = (STK_t *)topadr;
	if ( (wptr->next)->next == (STK_t *)((long)(wptr->next)+(wptr->next)->size) ) {
		(wptr->next)->next = ((wptr->next)->next)->next;
		(wptr->next)->size += ((wptr->next)->next)->size;
	}
	if ( wptr->next == (STK_t *)((long)(wptr)+wptr->size) ) {
		wptr->next = (wptr->next)->next;
		wptr->size += (wptr->next)->size;
	}
}

