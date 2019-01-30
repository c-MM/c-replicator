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

#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include <stdint.h>

#define RB_LENGTH 64

typedef uint8_t buffer_data_t;

typedef volatile struct {
	uint8_t		rb[RB_LENGTH];
	uint8_t		rb_in;
	uint8_t		rb_out;
	uint8_t		length;
} ring_buffer_t;

void rb_init(ring_buffer_t* buff);

void rb_clear(ring_buffer_t* buff);

void rb_put(ring_buffer_t* buff, buffer_data_t data);

buffer_data_t rb_get(ring_buffer_t* buff);

buffer_data_t rb_peek (ring_buffer_t* buff);

#endif
