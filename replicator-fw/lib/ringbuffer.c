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

#include <util/atomic.h>
#include "lib/ringbuffer.h"

void rb_init(ring_buffer_t* buff) {

	buff->rb_in		= 0;
	buff->rb_out	= 0;
	buff->length	= 0;
}

void rb_put (ring_buffer_t* buff, buffer_data_t data) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (buff->length < RB_LENGTH) {

			buff->rb[buff->rb_in] = data;
			buff->length++;
			buff->rb_in = (buff->rb_in + 1) % RB_LENGTH;
		}
	}
}

buffer_data_t rb_get (ring_buffer_t* buff) {
	buffer_data_t data = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (buff->length) {
			data = buff->rb[buff->rb_out];
			buff->length--;
			buff->rb_out = (buff->rb_out + 1) % RB_LENGTH;
		}
	}
	return data;
}

void rb_clear(ring_buffer_t* buff) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		buff->rb_out = buff->rb_in;
		buff->length = 0;
	}
}

buffer_data_t rb_peek (ring_buffer_t* buff) {
	buffer_data_t data = 0;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		data = buff->rb[buff->rb_out];
	}
	return data;
}
