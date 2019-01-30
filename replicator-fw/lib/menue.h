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

#ifndef _MENUE_H
#define _MENUE_H

#include <stdint.h>
#include "lib/timer1.h"

#define LCD_CHARS 16
#define LCD_BUF	(LCD_CHARS + 1)

extern char disp_line1[LCD_BUF], disp_line2[LCD_BUF];

typedef enum {
	MAINMENUE_NONE,
	MAINMENUE_OFF,
	MAINMENUE_READY,
	MAINMENUE_DISPENSE,
	MAINMENUE_ERRSTART,
	MAINMENUE_ERREND,
	MAINMENUE_EMPTY,
	MAINMENUE_BLOCKED,
	MAINMENUE_DISABLED,
	MAINMENUE_PRICE,
	MAINMENUE_DEBUG,
	MAINMENUE_SETUP,
	MAINMENUE_SERIAL,
} menue_main_state_t;

extern char disp_line1[LCD_BUF], disp_line2[LCD_BUF];
extern uint16_t menue_timeout;
extern times_t menue_time;

void menue_debug (void);
void menue_slots (void);
void menue_setup (void);
void menue_main (menue_main_state_t);

#endif
