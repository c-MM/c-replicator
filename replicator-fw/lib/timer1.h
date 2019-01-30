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

#ifndef _TIMER1_H
#define _TIMER1_H

#include <stdint.h>

typedef struct {
	uint32_t s;
	int16_t  ms;
	int16_t  offset;
} times_t;

#ifndef TIMER1
	#define TIMER1 is_used
#else
	#error TIMER01is already used by some other file
#endif


#ifndef TICKRATE
#define TICKRATE 100L
#endif

extern volatile uint8_t ticks;
extern volatile times_t time;

void timer1_init (void);
void timer1 (void);
void get_time(times_t *t);
int32_t elapsed (times_t *t);
uint32_t uptime (void);
#ifdef HAS_TIMER
	void timer1 (void);
#endif

#endif
