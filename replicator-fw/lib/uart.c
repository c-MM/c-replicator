/* vim: sw=4 ts=4 ai fdm=marker
 *  
 *
 *  copyright:  2009 - 2019 Martin Mueller (mm-at-sig21.net)
 *
 *  license:    This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU Library General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program. If not, see http:*www.gnu.org/licenses
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib/uart.h"
#include "lib/tasks.h"

volatile ring_buffer_t uart_tx;
volatile ring_buffer_t uart_rx;

int uart_putchar(uint8_t c, FILE *stream) {

	rb_put(&uart_tx, c);

	if ( !(UCSR0B & (1<<UDRIE0)) )
		UCSR0B |= (1<<UDRIE0);

	while (uart_tx.length == RB_LENGTH) {}

	return 0;
}

int uart_getchar(void) {

	if (uart_rx.length) {
		return (int)rb_get(&uart_rx);
	}
	return -1;
}

void uart_init(void) {

	rb_init(&uart_tx);
	rb_init(&uart_rx);

	if (BAUD > 19200) {
		UBRR0H = (unsigned char)((F_CPU/8/BAUD-1)>>8);
		UBRR0L = (unsigned char)(F_CPU/8/BAUD-1);
		UCSR0A = (1<<U2X0);
	} else {
		UBRR0H = (unsigned char)((F_CPU/16/BAUD-1)>>8);
		UBRR0L = (unsigned char)(F_CPU/16/BAUD-1);
		UCSR0A = 0;
	}
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0) ;
	//UCSR0B &= (~(1<<UDRIE0));
}

ISR(USART_UDRE_vect) {
	if (uart_tx.length) {
		while(!(UCSR0A & (1<<UDRE0))) {};
		UDR0 = rb_get(&uart_tx);
	} else {
		UCSR0B &= (~(1<<UDRIE0));
	}
}

ISR(USART_RX_vect) {
	rb_put(&uart_rx, UDR0);
	TASKS |= TASK_SERIALRX;
}
