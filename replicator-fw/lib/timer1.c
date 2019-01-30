/* vim: ts=4 sw=4 ai fdm=marker
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

#include <avr/io.h>
#include <util/atomic.h>
#include <stdio.h>
#include "board.h"
#include "lib/timer1.h"

#ifdef HAS_TIMER
#include "lib/timer.h"
#endif

volatile uint8_t ticks = 0;
volatile times_t time;

uint32_t uptime() {
	uint32_t up;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    	up = time.s;
	}
	return(up);
}

void get_time(times_t *t) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		t->s      = time.s;
		t->ms     = time.ms;
	}
	t->offset = time.offset;
}

int32_t elapsed(times_t *t) {
    int32_t x;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		x  = (int32_t)time.ms - t->ms;
		x += (time.s - t->s) * 1000;
	}
	return (int32_t)x;
}

#ifdef HAS_TIMER
void timer1() {
	uint8_t num;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		num = ticks;
	}

	while (num) {
		timertick();
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			ticks--;
			num = ticks;
		}
	}
}
#endif

void timer1_init() {
#ifdef HAS_TIMER
	ticks = 0;
#endif
	time.s = 0;
	time.ms = 0;
	TCCR1A = 0;								// disconnect OCR1x outputs, WGM1[01] zero
	TCCR1B = _BV(WGM12) | _BV(CS11);		// Use CTC mode with OCR1A, clk/8
	OCR1AH = (F_CPU / 8 / TICKRATE) >> 8;	// Set OCR1A to trigger TICKRATE per second
	OCR1AL = (F_CPU / 8 / TICKRATE) & 0xff;	// Set OCR1A to trigger TICKRATE per second
	TIMSK1 = _BV(OCIE1A);					// enable OCR1A interrupt
}

ISR(TIMER1_COMPA_vect) {
#ifdef HAS_TIMER
	// prevent misses events because of overflow 
	if (ticks < 255)
#endif
		ticks++;

	time.ms += 1000 / TICKRATE;
	if ( time.ms >= 1000 ) {
		time.s++;
		time.ms -= 1000;
	}
}
