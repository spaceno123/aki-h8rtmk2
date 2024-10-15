/* ADJ-702-137C (HITACH C compiler user's manual) p126 */

#include <stddef.h>

extern char *_s1ptr;
extern void srand(unsigned int);

void _INIT_OTHERLIB( void )
{
	srand(1);
	_s1ptr=NULL;
}

