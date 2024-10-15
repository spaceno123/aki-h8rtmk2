/*
	machine.h
*/

#ifndef _MACHINE_
#define _MACHINE_

#define set_imask_ccr(aaa) ((aaa)==0?and_ccr(0x3f):or_ccr(0xc0))

void set_ccr( unsigned char );
unsigned char get_ccr( void );
void and_ccr( unsigned char );
void or_ccr( unsigned char );
void xor_ccr( unsigned char );

#define sleep() for(;;)	/* スリープを無限ループで置き換えているので注意！ */

unsigned char get_and_ccr( unsigned char );	/* return before ccr data */
unsigned char get_or_ccr( unsigned char );	/* return before ccr data */
unsigned char get_xor_ccr( unsigned char );	/* return before ccr data */

#endif
