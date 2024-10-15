/* ADJ-702-137C (HITACH C compiler user's manual) p124 */

#include <stdlib.h>

const size_t _sbrk_size = 516;
extern void _INIT_LOWLEVEL(void);
extern void _INIT_IOLIB(void);
extern void _INIT_OTHERLIB(void);

void _INITLIB(void)
{
	errno = 0;

	_INIT_LOWLEVEL();
	_INIT_IOLIB();
	_INIT_OTHERLIB();
}

