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
 * uart.h
 *
 * Created: 21-Aug-2012 9:22:00 PM
 * Author: Sandipan Das
 */

#ifndef routines_uart_h_
#define routines_uart_h_

#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif

/*
 * constants and macros
 */

/** @brief  UART baudrate Expression
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600
 */
#define uart_baud_select(baudrate, xtal)				((xtal)/((baudrate)*16UL)-1)

/** @brief  UART baudrate Expression for ATmega double speed mode
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600
 */
#define uart_baud_select_double_speed(baudrate, xtal)	(((xtal)/((baudrate)*8UL)-1)|0x8000)

/** Size of the circular receive buffer, must be power of 2 */
#ifndef uart_rx_buffer_size
#define uart_rx_buffer_size	16
#endif
/** Size of the circular transmit buffer, must be power of 2 */
#ifndef uart_tx_buffer_size
#define uart_tx_buffer_size	16
#endif

/* test if the size of the circular buffers fits into SRAM */
#if ((uart_rx_buffer_size+uart_tx_buffer_size) >= (RAMEND-0x60 ))
#error "size of uart_rx_buffer_size + uart_tx_buffer_size larger than size of SRAM"
#endif

/* 
 * high byte error return code of uart_getc()
 */
#define uart_frame_error		0x0800	/* Framing Error by UART       */
#define uart_overrun_error		0x0400	/* Overrun condition by UART   */
#define uart_buffer_overflow	0x0200	/* receive ringbuffer overflow */
#define uart_no_data			0x0100	/* no receive data available   */

/*
 * function prototypes
 */

/**
 @brief   Initialize UART and set baudrate
 @param   baudrate Specify baudrate using macro uart_baud_select()
 @return  none
 */
extern void uart_init(unsigned int baudrate);

/**
 *  @brief   Get received byte from ringbuffer
 *
 * Returns in the lower byte the received character and in the 
 * higher byte the last receive error.
 * uart_no_data is returned when no data is available.
 *
 *  @param   void
 *  @return  lower byte:  received byte from ringbuffer
 *  @return  higher byte: last receive status
 *           - \b 0 successfully received data from UART
 *           - \b uart_no_data           
 *             <br>no receive data available
 *           - \b uart_buffer_overflow   
 *             <br>Receive ringbuffer overflow.
 *             We are not reading the receive buffer fast enough, 
 *             one or more received character have been dropped 
 *           - \b uart_overrun_error     
 *             <br>Overrun condition by UART.
 *             A character already present in the UART UDR register was 
 *             not read by the interrupt handler before the next character arrived,
 *             one or more received characters have been dropped.
 *           - \b uart_frame_error       
 *             <br>Framing Error by UART
 */
extern unsigned int uart_getc(void);

/**
 *  @brief   Put byte to ringbuffer for transmitting via UART
 *  @param   data byte to be transmitted
 *  @return  none
 */
extern void uart_putc(unsigned char data);

/**
 *  @brief   Put string to ringbuffer for transmitting via UART
 *
 *  The string is buffered by the uart library in a circular buffer
 *  and one character at a time is transmitted to the UART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 * 
 *  @param   s string to be transmitted
 *  @return  none
 */
extern void uart_puts(const char *s);

/**
 * @brief    Put string from program memory to ringbuffer for transmitting via UART.
 *
 * The string is buffered by the uart library in a circular buffer
 * and one character at a time is transmitted to the UART using interrupts.
 * Blocks if it can not write the whole string into the circular buffer.
 *
 * @param    s program memory string to be transmitted
 * @return   none
 * @see      uart_puts_P
 */
extern void uart_puts_p(const char *s);

/**
 * @brief    Macro to automatically put a string constant into program memory
 */
#define uart_puts_P(__s)       uart_puts_p(PSTR(__s))

#endif /* routines_uart_h_ */
