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
 * rftx.c
 *
 * Created: 04-Sep-2012 9:46:34 PM
 * Author: Sandipan Das
 */

#include <avr/io.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "rftx.h"
#include "utils.h"

static void tx_putc(uint8_t data) {
	/* wait until buffer is empty */
	while (!(UCSRA & (1 << UDRE)))
		;

	/* write data to buffer */UDR = data;

	/* wait until entire frame is shifted out */
	while (!(UCSRA & (1 << TXC)))
		;
}

void rftx_init(void) {
	/* set baud rate */
	UBRRL = (uint8_t) (UBRRVAL);
	UBRRH = (uint8_t) (UBRRVAL >> 8);

	/* enable transmitter only */UCSRB = (1 << TXEN);

	/* set frame format: asynchronous mode, 8-bit data, no parity, 1 stop bit  */
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
}

void tx_putcmd(uint8_t data) {
	uint8_t i;
	uint8_t packet[4];

	/* atomic transaction to prevent interrupts from interfering */
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		packet[0] = PACKET_HEAD;
		packet[1] = PACKET_SIGN;
		packet[2] = data;
		packet[3] = 0x00;
		switch (data) {
		case CMD_PWR:
			packet[3] = CRC_CMD_PWR;
			break;
		case CMD_INC:
			packet[3] = CRC_CMD_INC;
			break;
		case CMD_DEC:
			packet[3] = CRC_CMD_DEC;
			break;
		}

		tx_putc(0xFF);	// attempt to synchronize

		for (i = 0; i < 2; ++i) {
			tx_putc(packet[0]);
			tx_putc(packet[1]);
			tx_putc(packet[2]);
			tx_putc(0xFF);
		}

		tx_putc(packet[0]);
		tx_putc(packet[1]);
		tx_putc(packet[2]);
		tx_putc(packet[3]);
	}
}
