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

#ifndef _CHANGEGIVER_H
#define _CHANGEGIVER_H

#include <stdint.h>

void cg_init (void);
void cg_price (uint8_t price);
void cg_enable (uint8_t enable);
uint8_t cg_credit (void);
uint8_t cg_change_avail (void);

#endif
