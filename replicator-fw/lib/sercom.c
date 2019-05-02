/* vim: sw=4 ts=4 ai fdm=marker
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/uart.h"
#include "lib/timer1.h"
#include "lib/sercom.h"
#include "lib/tasks.h"

times_t remote_time;
uint16_t remote_timeout;


void serial_tx (uint8_t len, uint8_t* buf) {
	uint8_t chksum = 0;

	uart_putchar(0xAA, (FILE*)NULL);
	uart_putchar(len + 1 /* chksum */, (FILE*)NULL);
	chksum += (len + 1);
	for (uint8_t x = 0; x < len; x++) {
		uart_putchar(buf[x], (FILE*)NULL);
		chksum += buf[x];
	}
	uart_putchar(chksum, (FILE*)NULL);
}

uint8_t serial_setbaud(uint8_t* buf) {
	return 1;
}

uint8_t* serial_recv (void) {
	int chr;
	static uint8_t len;
	static uint8_t chksum;
	static uint8_t* buf = 0;
	static uint8_t expected = 0;
	static times_t start;

	if ( (chr = uart_getchar()) >= 0 ) {
		if ( expected ) {
			if ( elapsed(&start) > 1500 ) {
				if (buf)
					free(buf);
				 buf = 0;
				 expected = 0;
			}
			if ( len ) {
				if ( expected > 1 ) {
					chksum += (uint8_t)chr;
					*(buf+(len-expected+1)) = (uint8_t)chr;
				} else {
					if ( (uint8_t)chr != chksum ) {
						if (buf)
							free(buf);
						expected = 0;
						buf = 0;
						return NULL;
					}
				}
				expected--;
			} else {
				if ( (buf = malloc((uint8_t)chr)) == 0 ) {
					expected = 0;
					return NULL;
				}
				*buf = expected = chksum = len = (uint8_t)chr;
			}
		} else if ( (uint8_t) chr == 0xaa ) {
			get_time(&start);
			expected = 1;
			len = 0;
		} else {
			return NULL;
		}
	}

	if ( (!expected) && buf ) {
		uint8_t* ret = buf;
		buf = 0;
		return ret;
	}
	return NULL;
}

