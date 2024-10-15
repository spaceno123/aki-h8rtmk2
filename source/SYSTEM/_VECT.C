/* ADJ-702-137C (HITACH C compiler user's manual) p114 */

extern void SETUP(void);	/* INIT -> SETUP for set sp */
extern void i28x4(void);
extern void dbg_sci_err(void);
extern void dbg_sci_rxd(void);
extern void dbg_sci_txd(void);

#pragma section vect0x4

const void (*const vec_table0x4[])(void)={SETUP};

#pragma section vect28x4

const void (*const vec_table28x4[])(void)={i28x4};	/* itu1:1msec timer */

#pragma	section	vect56x4

const void (*const vec_table56x4[])(void)={	dbg_sci_err,
						dbg_sci_rxd,
						dbg_sci_txd};

