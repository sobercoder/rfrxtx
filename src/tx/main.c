/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * rfswitch_tx.c
 *
 * Created: 04-Sep-2012 9:20:21 PM
 * Author: Sandipan Das
 */

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "rftx.h"
#include "utils.h"

#define SW_INT_PORT	PORTD
#define SW_INT_PIN	PD3		// interrupt
#define SW_INP_PORT	PORTB
#define SW_PWR_PIN	PB5		// power on/off
#define SW_INC_PIN	PB4		// increase speed
#define SW_DEC_PIN	PB3		// decrease speed
#define TX_GND_PORT	PORTC
#define TX_GND_PIN	PC5

static inline void tx_pwr_on(void) {
	cbit(TX_GND_PORT, TX_GND_PIN);
	_delay_ms(50);
}
static inline void tx_pwr_off(void) {
	sbit(TX_GND_PORT, TX_GND_PIN);
}

ISR(INT1_vect) {
	/* test if power on/off switch is pressed */
	cbit(SW_INP_PORT, SW_PWR_PIN);
	sbit(SW_INP_PORT, SW_INC_PIN);
	sbit(SW_INP_PORT, SW_DEC_PIN);

	while (bit_is_clr(pin(SW_INT_PORT), SW_INT_PIN)) {
		cbit(SW_INP_PORT, SW_INC_PIN);
		cbit(SW_INP_PORT, SW_DEC_PIN);

		tx_pwr_on();
		tx_putcmd(CMD_PWR);
		tx_pwr_off();
	}

	/* test if increment switch is pressed */
	sbit(SW_INP_PORT, SW_PWR_PIN);
	cbit(SW_INP_PORT, SW_INC_PIN);
	sbit(SW_INP_PORT, SW_DEC_PIN);

	while (bit_is_clr(pin(SW_INT_PORT), SW_INT_PIN)) {
		cbit(SW_INP_PORT, SW_PWR_PIN);
		cbit(SW_INP_PORT, SW_DEC_PIN);

		tx_pwr_on();
		tx_putcmd(CMD_INC);
		tx_pwr_off();
	}

	/* test if decrement switch is pressed */
	sbit(SW_INP_PORT, SW_PWR_PIN);
	sbit(SW_INP_PORT, SW_INC_PIN);
	cbit(SW_INP_PORT, SW_DEC_PIN);

	while (bit_is_clr(pin(SW_INT_PORT), SW_INT_PIN)) {
		cbit(SW_INP_PORT, SW_PWR_PIN);
		cbit(SW_INP_PORT, SW_INC_PIN);

		tx_pwr_on();
		tx_putcmd(CMD_DEC);
		tx_pwr_off();
	}

	/* restore states */
	cbit(SW_INP_PORT, SW_PWR_PIN);
	cbit(SW_INP_PORT, SW_INC_PIN);
	cbit(SW_INP_PORT, SW_DEC_PIN);
}

int main(void) {
	/* initialize transmitter */
	rftx_init();

	/* setup external interrupts */
	cbit(ddr(SW_INT_PORT), SW_INT_PIN);
	// input
	sbit(ddr(SW_INP_PORT), SW_PWR_PIN);
	// output
	sbit(ddr(SW_INP_PORT), SW_INC_PIN);
	// output
	sbit(ddr(SW_INP_PORT), SW_DEC_PIN);
	// output
	sbit(ddr(TX_GND_PORT), TX_GND_PIN);
	// output

	sbit(SW_INT_PORT, SW_INT_PIN);
	// enable pullup
	cbit(SW_INP_PORT, SW_PWR_PIN);
	// disable pullup
	cbit(SW_INP_PORT, SW_INC_PIN);
	// disable pullup
	cbit(SW_INP_PORT, SW_DEC_PIN);
	// disable pullup

	sbit(GIMSK, INT1);
	// set interrupt mask
	tx_pwr_off();						// power off tx for now

	/* enable power saving features */
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable()
	;

	/* enable interrupts globally */sei();

	while (1) {
		/* enter sleep mode */
		sleep_cpu()
		;
	}
	return 0;
}
