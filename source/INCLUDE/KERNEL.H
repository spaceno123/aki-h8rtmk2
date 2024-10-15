/*
	Header	kernel.h
*/

#ifndef	_KERNEL_H_
#define	_KERNEL_H_

#include "include\itron.h"

#define	TA_HLNG		0x00
#define	TA_ASM		0x01

#define	TA_TFIFO	0x00
#define	TA_TPRI		0x01

#define	TA_MFIFO	0x00
#define	TA_MPRI		0x02

#define TA_ACT		0x02

#define	TSK_SELF	0
#define	TSK_NONE	0

#define	TMIN_TPRI	1
#define	TMAX_TPRI	16

#define	TMIN_MPRI	1
#define	TMAX_MPRI	16

#define	TMAX_ACTCNT	255

#define	TKERNEL_MAKER	0x0000
#define	TKERNEL_PRID	0x0000
#define	TKERNEL_SPVER	0x5401
#define	TKERNEL_PRVER	0x0001

/* --- オブジェクトの状態 --- */

#define	TTS_RUN	0x01		/* 実行状態 */
#define	TTS_RDY	0x02		/* 実行可能状態 */
#define	TTS_WAI	0x04		/* 待ち状態 */
#define TTS_SUS	0x08		/* 強制待ち状態 */
#define	TTS_WAS	0x0c		/* 二重待ち状態 */
#define	TTS_DMT	0x10		/* 休止状態 */

#define TTW_SLP	0x0001		/* 起床待ち状態 */
#define	TTW_DLY	0x0002		/* 時間経過待ち状態 */

/* --- パケット定義 --- */

typedef struct t_ctsk {		/* タスク生成情報のパケット形式 */
	ATR	tskatr;		/* タスク属性 */
	VP_INT	exinf;		/* タスクの拡張情報 */
	FP	task;		/* タスクの起動番地 */
	PRI	itskpri;	/* タスクの起動時優先度 */
	SIZE	stksz;		/* タスクのスタックサイズ（バイト数） */
	VP	stk;		/* タスクのスタック領域の先頭番地 */
} T_CTSK;

typedef struct t_rtsk {		/* タスク状態のパケット形式 */
	STAT	tskstat;	/* タスク状態 */
	PRI	tskpri;		/* タスクの現在優先度 */
	PRI	tskbpri;	/* タスクのベース優先度 */
	STAT	tskwait;	/* 待ち要因 */
	ID	wobjid;		/* 待ち対象のオブジェクトのＩＤ番号 */
	TMO	lefttmo;	/* タイムアウトするまでの時間 */
	UINT	actcnt;		/* 起動要求キューイング数 */
	UINT	wupcnt;		/* 起床要求キューイング数 */
	UINT	suscnt;		/* 強制待ち要求ネスト数 */
} T_RTSK;

typedef struct t_rtst {		/* タスク状態（簡易版）のパケット形式 */
	STAT	tskstat;	/* タスクの状態 */
	STAT	tskwait;	/* 待ち要因 */
} T_RSTS;

typedef struct t_dinh {		/* 割り込みハンドラ定義情報のパケット形式 */
	ATR	inhatr;		/* 割り込みハンドラ属性 */
	FP	inthdr;		/* 割り込みハンドラの起動番地 */
} T_DINH;

/* --- サービスコールプロトタイプ --- */

ER cre_tsk( ID, T_CTSK * );
ER act_tsk( ID );
ER iact_tsk( ID );
void ext_tsk( void );
ER dly_tsk( RELTIM );
ER isig_tim( void );
ER loc_cpu( void );
ER iloc_cpu( void );
ER unl_cpu( void );
ER iunl_cpu( void );
ER dis_dsp( void );
ER ena_dsp( void );
ER def_inh( INHNO, T_DINH * );

#endif
