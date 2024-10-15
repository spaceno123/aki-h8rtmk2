/*
	Header	vkernel.h
	Date	2002/7/5 .. 2002/8/15
	Copyright (C) 2002 by AKIYA
*/

#ifndef _VKERNEL_H_
#define _VKERNEL_H_

#include "include\itron.h"
#include "include\kernel.h"

/* --- カーネル管理機構 --- */

/* カーネルステート */
typedef struct vt_kst {
 UB	dspcnt;	/* ディスパッチカウンタ（！＝０：非タスクコンテキスト) */
 B	dspreq;	/* ディスパッチ要求状態（ＴＲＵＥ：ディスパッチ要求中） */
 B	dspdis;	/* ディスパッチ禁止状態（ＴＲＵＥ：ディスパッチ禁止中） */
 B	cpuloc;	/* ＣＰＵロック状態（ＴＲＵＥ：ＣＰＵロック状態） */
 UB	timreq;	/* システム時刻の更新リクエストカウンタ */
} VT_KST;

/* 双方向リンク */
typedef struct vt_lnk {
	struct vt_tcb	*fwd;	/* 前方リンク */
	struct vt_tcb	*bak;	/* 後方リンク */
} VT_LNK;

/* タスクコントロールブロック */
typedef struct vt_tcb {
	VT_LNK	lnk;		/* 双方向リンク */
	T_CTSK	ctsk;		/* タスク生成情報 */
	T_RTSK	rtsk;		/* タスク状態 */
	VT_CTX	ctxbuf;		/* コンテキストバッファ（ＣＰＵ依存） */
} VT_TCB;

/* --- ベンダ定義サービスコールプロトタイプ --- */

ER vcre_tsk( VT_TCB *, T_CTSK * );
ER vact_tsk( VT_TCB * );
void vext_tsk( void );
ER vdly_tsk( RELTIM );
ER ivsig_tim( void );
ER vloc_cpu( void );
ER vunl_cpu( void );
ER vdis_dsp( void );
ER vena_dsp( void );
ER vdef_inh( INHNO, T_DINH * );

void _kernel_inthdl_io_service( INHNO );
void _kernel_inthdl_null( void );

#endif
