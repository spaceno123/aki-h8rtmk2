/*
	Program	maintask.c
	Date	2002/8/4 .. 2002/8/20
	Copyright (C) 2002 by AKIYA
*/

#include "include\3048f.h"
#include "include\system.h"

#define MAIN_TCB( ent, pri, ssz ) VT_TCB main_tcb = {{NULL,NULL},\
	TA_HLNG,NULL,(ent),(pri),(ssz),NULL}

#define MAIN_TASK_PRI 1		/* 優先順位 */
#define MAIN_TASK_SSZ 256	/* スタックサイズ */
#define TEST_TASK_PRI 2		/* 優先順位 */
#define TEST_TASK_SSZ 256	/* スタックサイズ */

void main_task( VP_INT );
void test_task( VP_INT );

/* task tcb */
MAIN_TCB( main_task, MAIN_TASK_PRI, MAIN_TASK_SSZ );
VT_TCB test_tcb;

void main_task( VP_INT exinf )
{
	{
		T_CTSK	pk_ctsk;

		pk_ctsk.tskatr = TA_HLNG | TA_ACT;
		pk_ctsk.exinf = NULL;
		pk_ctsk.task = (VP)test_task;
		pk_ctsk.itskpri = TEST_TASK_PRI;
		pk_ctsk.stksz = TEST_TASK_SSZ;
		pk_ctsk.stk = NULL;
		cre_tsk( P2ID(&test_tcb), &pk_ctsk );
	}
	for ( ; ; ) {
		P5.DR.BIT.B0 ^= 1;
		dly_tsk(100);
	}
}

void test_task( VP_INT exinf )
{
	for ( ; ; ) {
		dly_tsk(500);
		P5.DR.BIT.B1 ^= 1;
	}
}
