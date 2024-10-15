/*
	Program	kernel.c
	Date	2002/7/5 .. 2002/8/16
	Copyright (C) 2002 by AKIYA

	サービスコールのエントリーをここで記述
	コンテキスト等のチェックとＩＤ→ポインタ変換
*/

#include "include\system.h"

extern volatile VT_KST _kernel_state;

/* --- タスク管理機能 --- */

/* タスクの生成 */
ER cre_tsk( ID tskid, T_CTSK *pk_ctsk )
{
	if ( _kernel_state.dspcnt != 0 )
		return E_CTX;
	else if ( _kernel_state.cpuloc == TRUE )
		return E_CTX;
	else
		return vcre_tsk( (VT_TCB *)ID2P(tskid), pk_ctsk );
}

/* タスクの起動 */
ER act_tsk( ID tskid )
{
	if ( _kernel_state.dspcnt != 0 )
		return E_CTX;
	else if ( _kernel_state.cpuloc == TRUE )
		return E_CTX;
	else
		return vact_tsk( (VT_TCB *)ID2P(tskid) );
}
ER iact_tsk( ID tskid )
{
	if ( _kernel_state.dspcnt == 0 )
		return E_CTX;
	else if ( _kernel_state.cpuloc == TRUE )
		return E_CTX;
	else
		return vact_tsk( (VT_TCB *)ID2P(tskid) );
}

/* 自タスクの終了 */
void ext_tsk( void )
{
	if ( _kernel_state.dspcnt == 0 )
		vext_tsk();
}

/* --- タスク付属同期機能 --- */

/* 自タスクの遅延 */
ER dly_tsk( RELTIM dlytim )
{
	if ( _kernel_state.dspcnt != 0 )
		return E_CTX;
	else if ( _kernel_state.cpuloc == TRUE )
		return E_CTX;
	else if ( _kernel_state.dspdis == TRUE )
		return E_CTX;
	else
		return vdly_tsk( dlytim );
}

/* --- 時間管理機能 --- */

/* タイムティックの供給 */
ER isig_tim( void )
{
	if ( _kernel_state.dspcnt == 0 )
		return E_CTX;
	else
		return ivsig_tim();
}

/* --- 割り込み管理機能 --- */

/* 割込みハンドラの定義 */
ER def_inh( INHNO inhno, T_DINH *pk_dinh )
{
	if ( _kernel_state.dspcnt != 0 )
		return E_CTX;
	else if ( inhno < MINVCTINO )
		return E_PAR;
	else if ( inhno > MAXVCTINO )
		return E_PAR;
	else
		return vdef_inh( inhno, pk_dinh );
}

/* --- システム状態管理機能 --- */

/* ＣＰＵロック状態への移行 */
ER loc_cpu( void )
{
	if ( _kernel_state.dspcnt != 0 )
		return E_CTX;
	else if ( _kernel_state.cpuloc == FALSE )
		return vloc_cpu();
	else
		return E_OK;
}
ER iloc_cpu( void )
{
	if ( _kernel_state.dspcnt == 0 )
		return E_CTX;
	else if ( _kernel_state.cpuloc == FALSE )
		return vloc_cpu();
	else
		return E_OK;
}

/* ＣＰＵロック状態の解除 */
ER unl_cpu( void )
{
	if ( _kernel_state.dspcnt != 0 )
		return E_CTX;
	else if ( _kernel_state.cpuloc == TRUE )
		return vunl_cpu();
	else
		return E_OK;
}
ER iunl_cpu( void )
{
	if ( _kernel_state.dspcnt == 0 )
		return E_CTX;
	else if ( _kernel_state.cpuloc == TRUE )
		return vunl_cpu();
	else
		return E_OK;
}

/* ディスパッチの禁止 */
ER dis_dsp( void )
{
	if ( _kernel_state.dspcnt != 0 )
		return E_CTX;
	else if ( _kernel_state.dspdis == FALSE )
		return vdis_dsp();
	else
		return E_OK;
}

/* ディスパッチの許可 */
ER ena_dsp( void )
{
	if ( _kernel_state.dspcnt != 0 )
		return E_CTX;
	else if ( _kernel_state.dspdis == TRUE )
		return vena_dsp();
	else
		return E_OK;
}

