/*
	Program	tmr_h8.c
	Date	2002/7/8 .. 2002/8/16
	Copyright (C) 2002 by AKIYA

	プロセッサ依存のシステム時刻の更新をここで記述
*/

#include "include\machine.h"

#include "include\system.h"
#include "include\3048f.h"

/* === １ｍｓｅｃ周期タイマー === */

#define CNT1MS 16000

void int_tmr();

/* １ｍｓｅｃ周期タイマー設定・起動（ＩＴＵ１を使用） */
void ini_tmr( void )
{
	T_DINH dinh;

	ITU.TSTR.BIT.STR1 = 0;	/* stop */
	dinh.inhatr = TA_HLNG;
	dinh.inthdr = int_tmr;
	def_inh( 28*4, &dinh );
	ITU1.TCR.BYTE = 0xa0;	/* CCLR=01:GRA,CKEG=00,TPSC=000:CLK(16MHz) */
	ITU1.GRA = CNT1MS-1;
	ITU1.TIER.BYTE = 0xf9;	/* OVIE=0,IMIEB=0,IMIEA=1 */
	ITU.TSTR.BIT.STR1 = 1;	/* start */
}

/* 割り込みハンドラ */
void int_tmr( void )
{
	static int flag = 0;			/* 再入抑止 */

	ITU1.TSR.BIT.IMFA &= 0;			/* 割り込み要因クリア */
	if ( flag++ == 0 ) {
		_kernel_cpu_unblock();		/* ブロック解除 */
		iunl_cpu();			/* 割り込み許可 */
		isig_tim();
	}
	flag--;
}
