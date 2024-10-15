/* ADJ-702-137C (HITACH C compiler user's manual) p123 */

#include "include\machine.h"

#pragma noregsave (INIT)

void main(void);
void _INITSCT(void);
void _INITLIB(void);
void _CLOSEALL(void);

void INIT(void)
/* コンパイラ未対応のため、SETUP.MAR にて実施
#pragma asm
	mov.l	#H'fffc00,sp
#pragma endasm
*/
{
	set_imask_ccr(0);
	_INITSCT();
	_INITLIB();
	main();
	_CLOSEALL();
	sleep();				/* 無限ループに展開 */
}
