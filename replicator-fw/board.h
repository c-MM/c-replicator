/* vim: sw=4 ts=4 ai fdm=marker
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
#ifndef __BOARD__H
#define __BOARD__H

#include <avr/io.h>

#define I2C_KEYS   0x00
#define I2C_LEDS   0x01
#define I2C_CRTL   0x02
#define I2C_MOTPOS 0x05
#define I2C_SLOTS  0x06
#define I2C_RELAIS 0x07

#define LED_OFF		0
#define LED_FAST	1
#define LED_MED		3
#define LED_SLOW	5
#define LED_ON		9

#define BAUD		9600
#define PARITY		odd

#endif /* __BOARD__H */
