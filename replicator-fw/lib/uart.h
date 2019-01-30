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

#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include <stdio.h>

#include "board.h"

#include "lib/ringbuffer.h"

extern volatile ring_buffer_t uart_tx;
extern volatile ring_buffer_t uart_rx;

void uart_init(void);

int uart_putchar(uint8_t c, FILE *stream);
int uart_getchar(void);

#endif
