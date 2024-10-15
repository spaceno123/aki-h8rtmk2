/* ADJ-702-137C (HITACH C compiler user's manual) p125 */

#define _NFILES 4

#include <stdio.h>

const int _nfiles = _NFILES;

struct {
	unsigned char *_bufptr;
	long _bufcnt;
	unsigned char *_bufbase;
	long _buflen;
	char _ioflag1;
	char _ioflag2;
	char _iofd;
} iob[_NFILES];

void _INIT_IOLIB( void )
{
	FILE *fp;

	for ( fp=_iob; fp<_iob+_nfiles; fp++) {
		fp->_bufptr = NULL;
		fp->_bufcnt = 0;
		fp->_buflen = 0;
		fp->_bufbase = NULL;
		fp->_ioflag1 = 0;
		fp->_ioflag2 = 0;
		fp->_iofd = 0;
	}
	if ( freopen("stdin", "r", stdin) == NULL )
		stdin->_ioflag1 = 0xff;
	stdin->_ioflag1 |= _IOUNBUF;
	if ( freopen("stdout", "w", stdout) == NULL )
		stdout->_ioflag1 = 0xff;
	stdout->_ioflag1 |= _IOUNBUF;
	if ( freopen("stderr","w",stderr) == NULL )
		stderr->_ioflag1 = 0xff;
	stderr->_ioflag1 |= _IOUNBUF;
}

