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

#include <stdio.h>
#include <avr/pgmspace.h>
#include "board.h"
#include "lib/menue.h"
#include "lib/tasks.h"
#include "lib/changegiver.h"
#include "lib/timer1.h"
#include "lib/config.h"


char disp_line1[LCD_BUF], disp_line2[LCD_BUF];
times_t menue_time;
uint16_t menue_timeout;

int8_t config_price(uint8_t price) {
	snprintf_P(disp_line1, LCD_BUF, PSTR("cetup preic %d   "), price + 1);
	snprintf_P(disp_line2, LCD_BUF, PSTR("%d.%02d            "), conf.price[price]/10, conf.price[price]%10*10);
	if ( i2c_keys & 0x01 ) return 1;
	if ( i2c_keys & 0x02 ) return 1;
	if ( i2c_keys & 0x04 && conf.price[price] < 50 ) conf.price[price]++;
	if ( i2c_keys & 0x08 && conf.price[price] > 0  ) conf.price[price]--;
	TASKS |= TASK_DISPLAY;
	return 0;
}

int8_t config_slot(uint8_t slot, uint8_t type) {
	if ( slot >= conf.slots )
		return 8-slot;

	uint8_t index = type ? conf.pr_member[slot] : conf.pr_alien[slot];
	uint8_t price = conf.price[index-1];
	snprintf_P(disp_line1, LCD_BUF, PSTR("chacht %d %6s "), slot + 1, type ? "member" : "alien");
	snprintf_P(disp_line2, LCD_BUF, PSTR("%d.%02d            "), price/10, price%10*10);
	if ( i2c_keys & 0x01 )  return type && (slot == conf.slots - 1) ? (8 - slot) * 2 : 1;
	if ( i2c_keys & 0x02 )  return type && (slot == conf.slots - 1) ? (8 - slot) * 2 : 1;
	if ( i2c_keys & 0x04 && index < 4 ) index++;
	if ( i2c_keys & 0x08 && index > 1 ) index--;
	if ( type ) 
		conf.pr_member[slot] = index;
	else
		conf.pr_alien[slot] = index;

	TASKS |= TASK_DISPLAY;
	return 0;
}

void menue_debug (void) {
	static uint16_t c;

	c++;
	snprintf(disp_line1, LCD_BUF, "%5u     IO %3d", c++, key_valid > 2 ? io_keys : 0);
	snprintf(disp_line2, LCD_BUF, "K%3d  S%3d  M%3d", key_valid > 2 ? i2c_keys : 0, i2c_slots, i2c_motpos);
}

void menue_slots (void) {
	int8_t slot;

	snprintf_P(disp_line1, LCD_BUF, PSTR("  chacht reset  "));
	snprintf_P(disp_line2, LCD_BUF, PSTR("                "));
	for ( slot = 0; slot < conf.slots; slot++ ) {
		if ( i2c_motpos & _BV(slot) )
			disp_line2[slot*2] = 'B';
		else if ( conf.slot_state & _BV(slot) )
			disp_line2[slot*2] = 'D';
		else if ( i2c_slots & _BV(slot) )
			disp_line2[slot*2] = 'E';
		else
			disp_line2[slot*2] = '_';
	}

	slot = slot_selected();
	if ( (slot > 0) && (slot <= conf.slots) ) {
		dispense_flag = DISPENSE_FORCE;
		TASKS |= TASK_DISPENSE;
	} else {
		dispense_flag = 0;
	}
	TASKS |= TASK_DISPLAY;
}

void menue_setup (void) {
	static uint16_t state;

	if ( state == 0 ) {
		snprintf_P(disp_line1, LCD_BUF, PSTR("cetup      1: ja"));
		snprintf_P(disp_line2, LCD_BUF, PSTR("2: nein  3:+ 4:-"));
		state = 1;
		TASKS |= TASK_DISPLAY;
		return;
	}

	if ( conf.mode == REP_SETUP ) {
		conf.mode = REP_READY;
		config_save();
	}

	if ( i2c_keys == last_keys )
		return;

	last_keys = i2c_keys;

	switch (state) {
	case 1:
		if ( i2c_keys & 0x01 ) state = 2;
		if ( i2c_keys & 0x02 ) { state = 0; REP_MODE = conf.mode; }
		snprintf_P(disp_line1, LCD_BUF, PSTR("cetup      1: ja"));
		snprintf_P(disp_line2, LCD_BUF, PSTR("2: nein  3:+ 4:-"));
		break;
	case 2:
		state += config_price(0);
		break;
	case 3:
		state += config_price(1);
		break;
	case 4:
		state += config_price(2);
		break;
	case 5:
		state += config_price(3);
		break;
	case 6:
		snprintf_P(disp_line1, LCD_BUF, PSTR(" cetup chaechte  "));
		snprintf_P(disp_line2, LCD_BUF, PSTR("%d               "), conf.slots);
		if ( i2c_keys & 0x01 ) state++;
		if ( i2c_keys & 0x02 ) state++;
		if ( i2c_keys & 0x04 && conf.slots < 8 ) conf.slots++;
		if ( i2c_keys & 0x08 && conf.slots > 0 ) conf.slots--;
		break;
	case 7:
		state += config_slot(0,0);
		break;
	case 8:
		state += config_slot(0,1);
		break;
	case 9:
		state += config_slot(1,0);
		break;
	case 10:
		state += config_slot(1,1);
		break;
	case 11:
		state += config_slot(2,0);
		break;
	case 12:
		state += config_slot(2,1);
		break;
	case 13:
		state += config_slot(3,0);
		break;
	case 14:
		state += config_slot(3,1);
		break;
	case 15:
		state += config_slot(4,0);
		break;
	case 16:
		state += config_slot(4,1);
		break;
	case 17:
		state += config_slot(5,0);
		break;
	case 18:
		state += config_slot(5,1);
		break;
	case 19:
		state += config_slot(6,0);
		break;
	case 20:
		state += config_slot(6,1);
		break;
	case 21:
		state += config_slot(7,0);
		break;
	case 22:
		state += config_slot(7,1);
		break;
	case 23:
	case 24:
		snprintf_P(disp_line1, LCD_BUF, PSTR("   cpeichern?   "));
		snprintf_P(disp_line2, LCD_BUF, PSTR(" 1:ja   2:nein  "));
		if ( i2c_keys & 0x01 ) config_save();
		if ( i2c_keys & 0x03 ) { state = 0; REP_MODE = conf.mode; }
		break;
	}

	TASKS |= TASK_DISPLAY;
}

void menue_main (menue_main_state_t state) {
	static menue_main_state_t last_state;

	if ( state == last_state )
		return;

	get_time(&menue_time);

	switch (state) {
	case MAINMENUE_OFF:
		menue_timeout = 0;
		snprintf_P(disp_line1, LCD_BUF, PSTR("   replicator   "));
		snprintf_P(disp_line2, LCD_BUF, PSTR("  auCer Betrieb "));
		break;
	case MAINMENUE_READY:
		menue_timeout = 0;
		snprintf_P(disp_line1, LCD_BUF, PSTR("   replicator   "));
		if (dispense_flag & DISPENSE_FREE)
			snprintf_P(disp_line2, LCD_BUF, PSTR("  Freiausgabe   "));
		else if (dispense_flag & DISPENSE_MEMBER)
			snprintf_P(disp_line2, LCD_BUF, PSTR("  memberpreice  "));
		else if ( ! cg_change_avail() )
			snprintf_P(disp_line2, LCD_BUF, PSTR("  paCend zahlen "));
		else
			snprintf_P(disp_line2, LCD_BUF, PSTR(" betriebcbereit "));
		break;
	case MAINMENUE_DISPENSE:
		menue_timeout = 0;
		snprintf_P(disp_line1, LCD_BUF, PSTR("  replication   "));
		snprintf_P(disp_line2, LCD_BUF, PSTR(" in bearbeitung "));
		break;
	case MAINMENUE_ERRSTART:
		menue_timeout = 5000;
		snprintf_P(disp_line1, LCD_BUF, PSTR("  replication   "));
		snprintf_P(disp_line2, LCD_BUF, PSTR("fehler bei ctart"));
		break;
	case MAINMENUE_ERREND:
		menue_timeout = 5000;
		snprintf_P(disp_line1, LCD_BUF, PSTR("  replication   "));
		snprintf_P(disp_line2, LCD_BUF, PSTR("fehler bei ende "));
		break;
	case MAINMENUE_EMPTY:
		menue_timeout = 5000;
		snprintf_P(disp_line1, LCD_BUF, PSTR(" fehler werc #%d "), slot_selected());
		snprintf_P(disp_line2, LCD_BUF, PSTR(" ceine reCourcen "));
		break;
	case MAINMENUE_BLOCKED:
		menue_timeout = 5000;
		snprintf_P(disp_line1, LCD_BUF, PSTR(" fehler werc #%d "), slot_selected());
		snprintf_P(disp_line2, LCD_BUF, PSTR(" ist blocciert   "));
		break;
	case MAINMENUE_DISABLED:
		menue_timeout = 5000;
		snprintf_P(disp_line1, LCD_BUF, PSTR(" fehler werc #%d "), slot_selected());
		snprintf_P(disp_line2, LCD_BUF, PSTR(" wird bestreicct "));
		break;
	case MAINMENUE_PRICE:
		menue_timeout = 2000;
		uint8_t slot = slot_selected();
		uint8_t alien = conf.price[conf.pr_alien[slot-1]-1];
		uint8_t member = conf.price[conf.pr_member[slot-1]-1];
		snprintf_P(disp_line1, LCD_BUF, PSTR(" zu wenig tocen "));
		snprintf_P(disp_line2, LCD_BUF, PSTR("  %d.%02d / %d.%02d   "), 
			member/10, (member%10) * 10, alien/10, (alien%10) * 10);
		break;
	case MAINMENUE_SERIAL:
		menue_timeout = 30000;
		return;
	default:
		break;
	}

	TASKS |= TASK_DISPLAY;
}
