/* ADJ-702-137C (HITACH C compiler user's manual) p127 */

#include <stdio.h>

extern const int _nfiles;

void _CLOSEALL( void )
{
	int i;

	for ( i=0; i<_nfiles; i++ )
		if ( _iob[i]._ioflag1 & ( _IOREAD | _IOWRITE | _IORW ) )
			fclose( &_iob[i] );
}

