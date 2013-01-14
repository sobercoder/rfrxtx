/*
 * 28-Jun-2012
 *
 * utils.h - Utility routines for several general purpose tasks.
 * 
 * This project is distributed under a GNU GPL v3 license.
 * See the included license.txt file for details.
 */

#ifndef utils_h_
#define utils_h_

/**
 * Some miscellaneous handy routines for tasks like bit manipulation.
 *
 * @author Sandipan Das
 * @version 0.1
 */

#ifndef outb
#define	outb(addr, data)		addr = (data)
#endif
#ifndef inb
#define	inb(addr)				(addr)
#endif
#ifndef outw
#define	outw(addr, data)		addr = (data)
#endif
#ifndef inw
#define	inw(addr)				(addr)
#endif
#ifndef bv
#define bv(bit)					(1 << (bit))
#endif
#ifndef cbit
#define cbit(reg, bit)			reg &= ~(bv(bit))
#endif
#ifndef sbit
#define sbit(reg, bit)			reg |=  (bv(bit))
#endif
#ifndef tbit
#define tbit(reg, bit)			reg ^=  (bv(bit))
#endif
#ifndef bit_is_set
#define	bit_is_set(reg, bit)	(reg & bv(bit))
#endif
#ifndef bit_is_clr
#define bit_is_clr(reg, bit)	(!(reg & bv(bit)))
#endif
#ifndef loop_until_bit_is_set
#define	loop_until_bit_is_set(reg, bit)		do { } while (bit_is_clr(reg, bit))
#endif
#ifndef loop_until_bit_is_clr
#define loop_until_bit_is_clr(reg, bit)		do { } while (bit_is_set(reg, bit))
#endif
#ifndef cli
#define cli()	__asm__ __volatile__ ("cli" ::)
#endif
#ifndef sei
#define sei()	__asm__ __volatile__ ("sei" ::)
#endif

/* port address helpers */
#define ddr(x)		(*(&x - 1))    // address of data direction register of port x
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
#define pin(x)		(&PORTF == &(x) ? _SFR_IO8(0x00) : (*(&x - 2)))    // on atmega64/128 PINF is on port 0x00 and not 0x60
#else
#define pin(x)		(*(&x - 2))    // address of input register of port x
#endif

/* utility macros */
#define min(a, b)		((a < b) ? (a) : (b))
#define max(a, b)		((a > b) ? (a) : (b))
#define concat(a, b)	((a)##(b))


#endif /* avrlibdefs_h_ */
