/* vim: sw=4 ts=4 ai fdm=marker
 *  
 *
 *  copyright:  2010 - 2011 Martin Mueller (mm-at-sig21.net)
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

#ifndef _SERCOM_H
#define _SERCOM_H

#include <stdint.h>

extern times_t remote_time;
extern uint16_t remote_timeout;

void serial_tx (uint8_t, uint8_t*);
uint8_t* serial_recv (void);

#endif
