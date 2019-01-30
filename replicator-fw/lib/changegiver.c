/* vim: ts=4 sw=4 ai fdm=marker
 *
 *
 *  copyright:  2019 Martin Mueller (mm-at-sig21.net)
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

#include <stdint.h>
#include <avr/io.h>
#include "board.h"

void cg_init (void) {

	DDRB  |= _BV(PB0);
	PORTB |= ( _BV(PB1) | _BV(PB2) );
	DDRC |= (_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) );

}

void cg_enable (uint8_t enable) {
	if (enable)
		PORTC |= _BV(PC3);
	else
		PORTC &= ~_BV(PC3);
}

uint8_t cg_credit (void) {
	if ( PINB & _BV(PB1) )
		return 1;
	return 0;
}

uint8_t cg_change_avail (void) {
	if ( PINB & _BV(PB2) )
		return 1;
	return 0;
}

void cg_price (uint8_t price) {
	switch (price) {
	case 0:
		PORTB &= ~_BV(PB0);
		PORTC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2));
		break;
	case 1:
		PORTB &= ~_BV(PB0);
		PORTC &= ~(_BV(PC1) | _BV(PC2));
		PORTC |= _BV(PC0);
		break;
	case 2:
		PORTB &= ~_BV(PB0);
		PORTC &= ~(_BV(PC0) | _BV(PC2));
		PORTC |= _BV(PC1);
		break;
	case 3:
		PORTB &= ~_BV(PB0);
		PORTC &= ~(_BV(PC0) | _BV(PC1));
		PORTC |= _BV(PC2);
		break;
	case 4:
		PORTC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2));
		PORTB |= _BV(PB0);
		break;
	}
}

