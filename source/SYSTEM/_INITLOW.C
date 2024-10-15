/*
	Program	_initlow.c
	Date	2002/6/30 .. 2002/6/30
	Copyright (C) 2002 by AKIYA

	入出力ポート定義（デバッガ初期化＆起動）

 up date
	2002/6/30	new
	2002/7/11	system.c -> _initlow.c
	2002/7/13	debuger initialize _init.c -> _initlow.c
*/

#include "include\3048f.h"
#include "include\machine.h"

#pragma noregsave _INIT_LOWLEVEL

void _INIT_LOWLEVEL( void )
{
	/* port 1 (out) */
	P1.DR.BYTE = 0x00;
	P1.DDR = 0xff;

	/* port 2 (in:bit7..0 S5-7..S5-0) */
	P2.DR.BYTE = 0x00;
	P2.DDR = 0x00;
	P2.PCR.BYTE = 0xff;

	/* port 3 (out:bit5..0 E,RS,DB7,DB6,DB5,DB4) */
	P3.DR.BYTE = 0x00;
	P3.DDR = 0xff;

	/* port 4 (in:bit7..4 SW4..1/out:bit3..0) */
	P4.DR.BYTE = 0x00;
	P4.DDR = 0x0f;
	P4.PCR.BYTE = 0xf0;

	/* port 5 (out:bit1,0 LED2,LED1) */
	P5.DR.BYTE = 0xf0;
	P5.DDR = 0xff;

	/* port 6 (out) */
	P6.DR.BYTE = 0x80;
	P6.DDR = 0xff;

	/* port 8 (out) */
	P8.DR.BYTE = 0xe0;
	P8.DDR = 0xff;

	/* port 9 (in:bit3,bit2 RXD1,RXD0/out:bit1,bit0 TXD1,TXD0) */
	P9.DR.BYTE = 0xc3;
	P9.DDR = 0xf3;

	/* port A (out) */
	PA.DR.BYTE = 0x00;
	PA.DDR = 0xff;

	/* port B */
	PB.DR.BYTE = 0x00;
	PB.DDR = 0xff;

	set_imask_ccr(1);			/* 割り込み禁止 */
	dbg_init();				/* デバッガ初期化 */
	set_imask_ccr(0);			/* 割り込み許可 */
/*	dbg_mode_in();				/* デバッガモード起動 */
}
