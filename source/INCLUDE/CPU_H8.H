/*
	Header	cpu_h8.h
	Date	2002/7/5 .. 2002/8/16
	Copyright (C) 2002 by AKIYA

	プロセッサに依存する要素をここで定義
*/

#ifndef	_CPU_H8_H_
#define	_CPU_H8_H_

#include "include\machine.h"

/* ＩＤと構造体ポインタの関係マクロ */
/* （構造体のポインタをそのままＩＤとして使用する為） */
#define	ID2P(aaa)	((VP)(aaa))
#define	P2ID(aaa)	((W)(aaa))

/* 割り込みベクタと割り込みナンバーの関係マクロ */
#define MINVCTINO	(8*4)
#define	MAXVCTINO	(60*4)
#define	VCT2INO(aaa)	(((aaa)-MINVCTINO)/4)
#define	INO2VCT(aaa)	(((aaa)*4)+MINVCTINO)

/* 割り込み禁止・許可マクロ */
#define	_kernel_cpu_block()
#define	_kernel_cpu_unblock()
#define	_kernel_int_disable()	set_imask_ccr(1)
#define	_kernel_int_enable()	set_imask_ccr(0)

/* 開始アドレスセット */
#define	_kernel_set_adr(aaa,bbb)	((aaa)->ctxbuf.ret=(bbb))
#define _kernel_set_stk(aaa,bbb)	((aaa)->ctxbuf.sp=(bbb))

/* コンテキストバッファ */
typedef struct vt_ctx {
	long	sp;
	long	ret;
	long	er1;
	long	er2;
	long	er3;
	long	er4;
	long	er5;
	long	er6;
} VT_CTX;

#endif	/* _CPU_H8_H_ */
