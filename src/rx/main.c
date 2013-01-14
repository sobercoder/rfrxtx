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
 * main.c
 *
 * Created: 05-Sep-2012 11:57:31 AM
 * Author: Sandipan Das
 */

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "rfrx.h"
#include "utils.h"

uint8_t EEMEM state = 0;	// default state is OFF
uint8_t EEMEM speed = 0;	// default speed is 1

volatile static uint8_t cur_state = 0;
volatile static uint8_t cur_speed = 0;

void init(void) {
	rx_init();			// initialize receiver

	eeprom_busy_wait();
	cur_state = eeprom_read_byte(&state);	// restore state
	eeprom_busy_wait();
	cur_speed = eeprom_read_byte(&speed);	// restore speed
}

int main(void) {
	init();
	sei();	// enable interrupts globally

	while (1) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			uint8_t cmd = rx_getcmd();
			switch (cmd) {
			case CMD_PWR:
				tbit(cur_state, 0);
				break;
			case CMD_INC:
				if (cur_speed != 9) {
					++cur_speed;
				}
				break;
			case CMD_DEC:
				if (cur_speed != 0) {
					--cur_speed;
				}
				break;
			}
		}
	}
	return 0;
}
