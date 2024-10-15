/* ADJ-702-137C (HITACH C compiler user's manual) p235..p238 */

#include <string.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define FLMIN 0
#define FLMAX 3

#define O_RDONLY 0x0001
#define O_WRONLY 0x0002
#define O_RDWR 0x0004

#define CR 0x0d
#define LF 0x0a

#define HEAPSIZE 2064

extern void charput( char );
extern char charget( void );

char flmod[FLMAX];

static union {
	short dummy;
	char heap[HEAPSIZE];
} heap_area;

static char *brk=(char *)&heap_area;

int open( char *name, int mode, int flg )
{
	if ( strcmp(name, "stdin") == 0 ) {
		if ( (mode & O_RDONLY) == 0 )
			return -1;
		flmod[STDIN] = mode;
		return STDIN;
	} else if ( strcmp(name, "stdout") == 0 ) {
		if ( (mode & O_WRONLY) == 0 )
			return -1;
		flmod[STDOUT] = mode;
		return STDOUT;
	} else if ( strcmp(name, "stderr") == 0 ) {
		if ( (mode & O_WRONLY) == 0 )
			return -1;
		flmod[STDERR] = mode;
		return STDERR;
	} else
		return -1;
}

int close( int fileno )
{
	if ( (fileno < FLMIN) || (FLMAX <= fileno) )
		return -1;
	flmod[fileno] = 0;
	return 0;
}

int read( int fileno, char *buf, int count )
{
	int i;

	if ( (flmod[fileno] & O_RDONLY) || (flmod[fileno] & O_RDWR) ) {
		for ( i = count; i > 0; i-- ) {
			*buf = charget();
			if ( *buf == CR ) *buf = LF;	/* for console ? */
			buf++;
		}
		return count;
	} else
		return -1;
}

int write( int fileno, char *buf, int count )
{
	int i;
	char c;

	if ( (flmod[fileno] & O_WRONLY) || (flmod[fileno] & O_RDWR) ) {
		for ( i = count; i > 0; i-- ) {
			c = *buf++;
			if ( c == LF ) charput( CR );	/* for console */
			charput( c );
		}
		return count;
	} else
		return -1;
}

long lseek( int fileno, long offset, int base )
{
	return -1l;
}

char *sbrk( int size )
{
	char *p;

	if ( brk+size>heap_area.heap+HEAPSIZE )
		return (char *)-1;
	p = brk;
	brk += size;
	return p;
}
