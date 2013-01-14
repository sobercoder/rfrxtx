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
 * rfrx.c
 *
 * Created: 05-Sep-2012 11:59:03 AM
 * Author: Sandipan Das
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "rfrx.h"

#define PACKET_SIZE		4
#define RX_BUFFER_SIZE	2
#define RX_BUFFER_MASK	(RX_BUFFER_SIZE - 1)

/* size of the circular transmit buffer, must be power of 2 */
#define TX_BUFFER_SIZE	8
#define TX_BUFFER_MASK	(TX_BUFFER_SIZE - 1)

/* test if the size of the circular buffers fits into SRAM */
#if ((PACKET_SIZE+TX_BUFFER_SIZE) >= (RAMEND-0x60 ))
#error "size of buffers larger than size of SRAM"
#endif

static volatile uint8_t packet[PACKET_SIZE];
static volatile uint8_t rx_buf[RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

static volatile uint8_t has_head = 0;
static volatile uint8_t has_sign = 0;
static volatile uint8_t has_data = 0;
static volatile uint8_t has_crc8 = 0;

static volatile uint8_t cur_data = 0;
static volatile uint8_t cur_crc8 = 0;

void rx_init(void) {
	/* set baud rate */UBRRL = (uint8_t) (UBRRVAL);
	UBRRH = (uint8_t) (UBRRVAL >> 8);
	/* enable receiver only */UCSRB = (1 << RXCIE) | (1 << RXEN);
	/* set frame format: asynchronous mode, 8-bit data, no parity, 1 stop bit  */
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
}

uint8_t rx_getcmd(void) {
	uint8_t tmptail;
	uint8_t data;

	if (rx_head == rx_tail) {
		return 0;	// no data available
	}

	tmptail = (rx_tail + 1) & RX_BUFFER_MASK;	// calculate buffer index
	rx_tail = tmptail;							// store buffer index
	data = rx_buf[tmptail];						// get data from buffer

	return data;
}

static void rx_flush(void) {
	has_head = 0;
	has_sign = 0;
	has_data = 0;
	has_crc8 = 0;

	cur_data = 0;
	cur_crc8 = 0;
}

/* interrupt service routine for receiving data */ISR(USART_RXC_vect) {
	uint8_t data;
	uint8_t crc8;
	uint8_t tmphead;

	data = UDR;	// read data register
	crc8 = 0x00;

	if (data == PACKET_HEAD || data == PACKET_SIGN || data == CMD_PWR
			|| data == CMD_INC || data == CMD_DEC || data == CRC_CMD_PWR
			|| data == CRC_CMD_INC || data == CRC_CMD_DEC) {

		if (data == PACKET_HEAD) {
			has_head = 1;
			packet[0] = data;
		} else if (data == PACKET_SIGN) {
			if (has_head) {
				has_sign = 1;
				packet[1] = data;
			}
		} else if (data == CMD_PWR || data == CMD_INC || data == CMD_DEC) {
			if (has_sign) {
				has_data = 1;
				cur_data = data;
				packet[2] = data;
			}
		} else if (data == CRC_CMD_PWR || data == CRC_CMD_INC
				|| data == CRC_CMD_DEC) {
			if (has_data) {
				has_crc8 = 1;
				cur_crc8 = data;

				switch (cur_data) {
				case CMD_PWR:
					crc8 = CRC_CMD_PWR;
					break;
				case CMD_INC:
					crc8 = CRC_CMD_INC;
					break;
				case CMD_DEC:
					crc8 = CRC_CMD_DEC;
					break;
				}

				if (cur_crc8 == crc8) {
					tmphead = (rx_head + 1) & RX_BUFFER_MASK;// calculate buffer index
					if (tmphead != rx_tail) {
						rx_head = tmphead;					// store new index
						rx_buf[tmphead] = cur_data;		// store data in buffer
					}
				}

				rx_flush();
			}
		}
	} else {
		rx_flush();
	}
}
