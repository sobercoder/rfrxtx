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
 * uart.c
 *
 * Created: 21-Aug-2012 9:30:03 PM
 * Author: Sandipan Das
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"

/*
 *  constants and macros
 */

/* size of RX/TX buffers */
#define uart_rx_buffer_mask	(uart_rx_buffer_size - 1)
#define uart_tx_buffer_mask	(uart_tx_buffer_size - 1)

#if (uart_rx_buffer_size & uart_rx_buffer_mask)
#error RX buffer size is not a power of 2
#endif
#if (uart_tx_buffer_size & uart_tx_buffer_mask)
#error TX buffer size is not a power of 2
#endif

/* ATmega8 with one USART */
#define uart_receive_interrupt	USART_RXC_vect
#define uart_transmit_interrupt	USART_UDRE_vect
#define uart_status				UCSRA
#define uart_control			UCSRB
#define uart_data				UDR
#define uart_udrie				UDRIE

/*
 *  module global variables
 */
static volatile unsigned char uart_tx_buf[uart_tx_buffer_size];
static volatile unsigned char uart_rx_buf[uart_rx_buffer_size];
static volatile unsigned char uart_tx_head;
static volatile unsigned char uart_tx_tail;
static volatile unsigned char uart_rx_head;
static volatile unsigned char uart_rx_tail;
static volatile unsigned char uart_last_rx_err;

ISR(uart_transmit_interrupt) {
	unsigned char tmptail;

	if (uart_tx_head != uart_tx_tail) {
		/* calculate and store new buffer index */
		tmptail = (uart_tx_tail + 1) & uart_tx_buffer_mask;
		uart_tx_tail = tmptail;
		/* get one byte from buffer and write it to UART */
		uart_data = uart_tx_buf[tmptail]; /* start transmission */
	} else {
		/* tx buffer empty, disable UDRE interrupt */
		uart_control &= ~_BV(uart_udrie);
	}
}

void uart_init(unsigned int baudrate) {
	uart_tx_head = 0;
	uart_tx_tail = 0;
	uart_rx_head = 0;
	uart_rx_tail = 0;

	/* set baud rate */
	if (baudrate & 0x8000) {
		uart_status = (1 << U2X); /* enable 2x speed */
		baudrate &= ~0x8000;
	}
	UBRRH = (unsigned char) (baudrate >> 8);
	UBRRL = (unsigned char) (baudrate);

	/* enable USART receiver and transmitter and receive complete interrupt */uart_control =
			_BV(RXCIE) | _BV(RXEN) | _BV(TXEN);

	/* set frame format: asynchronous, 8data, no parity, 1stop bit */
#ifdef URSEL
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
#else
	UCSRC = (3<<UCSZ0);
#endif 
}

unsigned int uart_getc(void) {
	unsigned char tmptail;
	unsigned char data;

	if (uart_rx_head == uart_rx_tail) {
		return uart_no_data; /* no data available */
	}

	/* calculate /store buffer index */
	tmptail = (uart_rx_tail + 1) & uart_rx_buffer_mask;
	uart_rx_tail = tmptail;

	/* get data from receive buffer */
	data = uart_rx_buf[tmptail];

	return (uart_last_rx_err << 8) + data;
}

void uart_putc(unsigned char data) {
	unsigned char tmphead = (uart_tx_head + 1) & uart_tx_buffer_mask;

	while (tmphead == uart_tx_tail) {
		; /* wait for free space in buffer */
	}

	uart_tx_buf[tmphead] = data;
	uart_tx_head = tmphead;

	/* enable UDRE interrupt */
	uart_control |= _BV(uart_udrie);
}

void uart_puts(const char *s) {
	while (*s) {
		uart_putc(*s++);
	}
}

void uart_puts_p(const char *progmem_s) {
	register char c;

	while ((c = pgm_read_byte(progmem_s++))) {
		uart_putc(c);
	}
}
