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
#include <stdio.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "board.h"
#include "lib/lcd.h"
#include "lib/menue.h"
#include "lib/tasks.h"
#include "lib/pcf8574.h"
#include "lib/timer1.h"
#include "lib/changegiver.h"
#include "lib/config.h"
#include "lib/sercom.h"


volatile uint16_t TASKS;
replicator_mode_t REP_MODE;
uint8_t	last_keys, key_valid, io_keys, i2c_keys, i2c_slots, i2c_motpos, dispense_flag;
uint8_t leds[10];

int8_t slot_selected (void) {
	uint8_t slot = 0;

	if ( ! (i2c_keys && !(i2c_keys & (i2c_keys-1))) )
		return -1;

	while ( (i2c_keys & 1<<slot) == 0 ) {
		slot++;
	}

	if ( slot > (conf.slots - 1 ) )
		return -1;
	
	return slot;
}

static void task_serialrx (void) {
	uint8_t len;
	uint8_t answer[80];
	static uint8_t* buf;
	uint8_t x, y;

	buf = serial_recv();

	if (buf) {
		switch (buf[1]) {
		case 0x30: // member prices
			if ( buf[2] == 0x00 )
				dispense_flag &= ~DISPENSE_MEMBER;
			if ( buf[2] == 0x01 ) {
				dispense_flag |= DISPENSE_MEMBER;
				get_time(&remote_time);
				remote_timeout = 30000;
			}
			menue_timeout = 2000;
			break;
		case 0x31: // display text
			if ( ( buf[2] == 1 ) || buf[2] == 2 )
				lcd_clearline(buf[2]-1);

			for ( x = 3; x < buf[0]; x++ ) {
				lcd_char(buf[x]);
			}
			get_time(&remote_time);
			remote_timeout = 10000;
			menue_main(MAINMENUE_SERIAL);
			break;
		case 0x32: // cancel member prices
			dispense_flag &= ~DISPENSE_MEMBER;
			menue_main(MAINMENUE_READY);
			break;
		case 0x34: // free dispense
			if ( buf[2] == 0x00 )
				dispense_flag &= ~DISPENSE_FREE;
			if ( buf[2] == 0x01 ) {
				dispense_flag |= DISPENSE_FREE;
				get_time(&remote_time);
				remote_timeout = 30000;
			}
			break;
		case 0x35: // query counters
			len = 1 + 3 + conf.slots * 2 * 4;
			x = 0;
			answer[x++] = 0x06;
			answer[x++] = conf.slots;
			answer[x++] = 4; // num counters per slot (alien, member, casse, error)
			answer[x++] = 2; // bytes per counter
			for ( y = 0; y < conf.slots; y++ ) { answer[x++] = eecounter.alien[y]  & 0x00ff;  answer[x++] = eecounter.alien[y]  >> 8; }
			for ( y = 0; y < conf.slots; y++ ) { answer[x++] = eecounter.member[y] & 0x00ff;  answer[x++] = eecounter.member[y] >> 8; }
			for ( y = 0; y < conf.slots; y++ ) { answer[x++] = eecounter.casse[y]  & 0x00ff;  answer[x++] = eecounter.casse[y]  >> 8; }
			for ( y = 0; y < conf.slots; y++ ) { answer[x++] = eecounter.error[y]  & 0x00ff;  answer[x++] = eecounter.error[y]  >> 8; }
			serial_tx(len, answer);
			break;
		case 0x36: // query status
			len = 1 + 2 + conf.slots;
			x = 0;
			answer[x++] = 0x07;
			answer[x++] = conf.slots;
			answer[x++] = (REP_MODE == REP_READY ? 1 : 0) | (cg_change_avail() << 1);
			for ( y = 0; y < conf.slots; y++ ) {
				answer[x++] = ( ((~i2c_slots & (1<<y)) >> y) | (((~i2c_motpos & (1<<y)) >> y) << 1) );
			}
			serial_tx(len, answer);
			break;
		}
		free(buf);
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		TASKS &= ~TASK_SERIALRX;
	}
}

static void task_dispense (void) {
	uint8_t slot;
	static times_t start;
	static dispense_state_t state;

	slot = slot_selected();

	switch (state) {
	case NONE:
		if ( ! (dispense_flag & DISPENSE_FORCE) ) {
			TASKS &= ~TASK_DISPENSE;
			if ( conf.slot_state & _BV(slot) ) {
				menue_main(MAINMENUE_DISABLED);
				return;
			} else if ( i2c_motpos & _BV(slot) ) {
				menue_main(MAINMENUE_BLOCKED);
				return;
			} else if ( i2c_slots & _BV(slot) ) {
				menue_main(MAINMENUE_EMPTY);
				return;
			} else {
				TASKS |= TASK_DISPENSE;
				if ( !dispense_flag )
					cg_price(conf.pr_alien[slot]);
				else if ( dispense_flag & DISPENSE_MEMBER )
					cg_price(conf.pr_member[slot]);
			}
		}
		get_time(&start);
		pcf8574_set_outputs(I2C_CRTL,0xff); // triac trigger off, monoflop reset off
		state = STARTING;
		break;
	case STARTING:
		// As per MEI Cashflow 560 applications design guide we need to wait 35ms 
		// for the changegiver to assert the price relais (and there by switching 
		// the Safety Line to open drain
		if ( elapsed(&start) < 35 )
			break;
		if ( (dispense_flag & (DISPENSE_FREE|DISPENSE_FORCE)) || cg_credit() ) {
			// start the aaction if there is enough money or it is free or forced
			state = STARTED;
			if ( ! (dispense_flag & DISPENSE_FORCE) )
				menue_main(MAINMENUE_DISPENSE);
			pcf8574_set_outputs(I2C_RELAIS,_BV(slot)); // slot relais on
			pcf8574_set_outputs(I2C_CRTL,0x7f); // trigger triac
			leds[slot] = LED_FAST;
			break;
		}
		// Not enough money, stop the action
		menue_main(MAINMENUE_PRICE);
		state = ERROR;
		break;
	case STARTED:
		if ( i2c_motpos & _BV(slot) ) {
			state = RUNNING;
			if ( ! (dispense_flag & (DISPENSE_FORCE | DISPENSE_FREE)) )
				// Motor did start, collect the money, we have only 2.5s for that
				cg_enable(0);
			if ( dispense_flag & DISPENSE_FREE )
				eecounter.casse[slot]++;
			else if ( dispense_flag & DISPENSE_MEMBER )
				eecounter.member[slot]++;
			else if ( ! (dispense_flag & DISPENSE_FORCE) )
				eecounter.alien[slot]++;
			counter_save();
			dispense_flag &= ~DISPENSE_MEMBER;
			dispense_flag &= ~DISPENSE_FREE;
		}
		if ( elapsed(&start) > 2000 ) {
			// Error, motor couldn't start
			if ( ! (dispense_flag & DISPENSE_FORCE) )
				menue_main(MAINMENUE_ERRSTART);
			conf.slot_state |= _BV(slot);
			config_save();
			state = ERROR;
		}
		break;
	case RUNNING:
		if ( elapsed(&start) > 15000 ) {
			// Error, motor couldn't finish
			if ( ! (dispense_flag & DISPENSE_FORCE) )
				menue_main(MAINMENUE_ERREND);
			state = ERROR;
			break;
		}
		if ( ! (i2c_motpos & _BV(slot)) )
			state = FINISHED;
		break;
	case ERROR:
		TASKS &= ~TASK_DISPENSE;
		if ( ! (dispense_flag & DISPENSE_FORCE) ) {
			eecounter.error[slot]++;
			counter_save();
		}
		break;
	case FINISHED:
		// Success, motor is back on zero position
		if ( ! (dispense_flag & DISPENSE_FORCE) )
			menue_main(MAINMENUE_READY);
		if ( conf.slot_state & _BV(slot) ) {
			conf.slot_state &= ~_BV(slot);
			config_save();
		}
		TASKS &= ~TASK_DISPENSE;
		break;
	}

	if ( ! (TASKS & TASK_DISPENSE) ) {
		pcf8574_set_outputs(I2C_CRTL,0xbf); // triac trigger off, monoflop reset on
		pcf8574_set_outputs(I2C_RELAIS,0);  // relais off
		cg_price(0);
		cg_enable(1);
		state = NONE;
		TASKS |= TASK_SETLEDS;
		TASKS |= TASK_DISPLAY;
		return;
	}

	TASKS |= TASK_MOTPOS;
}

static void task_motpos (void) {
	static uint8_t i2c_motpos_last, valid;
	uint8_t local_i2c_motpos;

	TASKS &= ~TASK_MOTPOS;
	local_i2c_motpos = pcf8574_get_inputs(I2C_MOTPOS);

	if ( local_i2c_motpos == i2c_motpos_last ) {
		if ( valid < 255 )
			valid++;

		if ( (i2c_motpos != local_i2c_motpos) && (valid > 2) ) {
			i2c_motpos = local_i2c_motpos;
			TASKS |= TASK_DISPLAY;
			TASKS |= TASK_SETLEDS;
		}

	} else
		valid = 0;

	i2c_motpos_last = local_i2c_motpos;
}

static void task_setleds (void) {
	uint8_t slot;

	TASKS &= ~TASK_SETLEDS;
	for (slot = 0; slot < conf.slots; slot++) {
		if ( conf.slot_state & _BV(slot) )
			leds[slot] = LED_SLOW;
		else if ( i2c_motpos & _BV(slot) )
			leds[slot] = LED_MED;
		else if ( i2c_slots & _BV(slot) )
			leds[slot] = LED_ON;
		else
			leds[slot] = LED_OFF;
	}

	if ( cg_change_avail() )
		leds[9] = LED_OFF;
	else
		leds[9] = LED_ON;
}

static void task_slots (void) {
	static uint8_t i2c_slots_last;

	TASKS &= ~TASK_SLOTS;
	i2c_slots  = pcf8574_get_inputs(I2C_SLOTS);
	if ( i2c_slots != i2c_slots_last )
		TASKS |= TASK_SETLEDS;
	i2c_slots_last = i2c_slots;

}
	
static void task_keys (void) {
	static uint8_t i2c_keys_last;
	static uint8_t io_keys_last;
	uint8_t local_i2c_keys, local_io_keys;

	TASKS &= ~TASK_KEYS;

	local_i2c_keys = ~pcf8574_get_inputs(I2C_KEYS);
	local_io_keys  = (~PINB & 0x18) >> 3;
	if ( (i2c_keys == i2c_keys_last) && (io_keys == io_keys_last) ) {
		if (key_valid < 255) 
			key_valid++;

		if ( (i2c_keys != local_i2c_keys) || (io_keys != local_io_keys) ) {
			if (key_valid > 2) {
				i2c_keys = local_i2c_keys;
				io_keys  = local_io_keys;
				TASKS |= TASK_DISPLAY;
			}
		}
	} else
		key_valid = 0;
	
	i2c_keys_last = local_i2c_keys;
	io_keys_last  = local_io_keys;
}

static void task_leds (void) {
	static uint8_t c;
	uint16_t out = 0;
	uint8_t  x, tmp;

	TASKS &= ~TASK_LEDS;
	for (x = 0; x < 10; x++) {
		if ( leds[x] )
			out |= ( (~(c >> (leds[x]-1))) & 0x01) << x;
	}
	//pcf8574_set_outputs(0x01,(1<<c) ^ (i2c_slots | i2c_motpos));
	pcf8574_set_outputs(I2C_LEDS,out & 0x00FF);
	tmp = pcf8574_get_inputs(I2C_CRTL) & 0xfc;
	tmp |= (out >> 8) & 0x03;
	pcf8574_set_outputs(I2C_CRTL,tmp);
	c++;
}

static void task_display (void) {

	TASKS &= ~TASK_DISPLAY;

	lcd_pos(0,0);
	//sprintf(str, "%5u     IO %3d", c++, key_valid > 2 ? io_keys : 0);
	lcd_write_s(disp_line1);

	lcd_pos(1,0);
	//sprintf(str, "K%3d  S%3d  M%3d", key_valid > 2 ? i2c_keys : 0, i2c_slots, i2c_motpos);
	lcd_write_s(disp_line2);
}

void handle_tasks() {

	if ( TASKS & TASK_MOTPOS )
		task_motpos();

	if ( TASKS & TASK_LEDS )
		task_leds();

	if ( TASKS & TASK_DISPLAY )
		task_display();

	if ( TASKS & TASK_DISPENSE ) {
		task_dispense();
		// We are not interested in anything else while dispensing
		return;
	}

	if ( TASKS & TASK_KEYS )
		task_keys();

	if ( TASKS & TASK_SLOTS )
		task_slots();

	if ( TASKS & TASK_SETLEDS )
		task_setleds();
	
	if ( TASKS & TASK_SERIALRX )
		task_serialrx();

	if ( TASKS )
		// Handle pending tasks before doing more fancy stuff
		return;

	if ( remote_timeout && (elapsed(&remote_time) >= remote_timeout) ) {
		serial_tx(1, (uint8_t*)"\x05");
		remote_timeout = 0;
		dispense_flag &= ~DISPENSE_MEMBER;
		dispense_flag &= ~DISPENSE_FREE;
		menue_timeout = 0;
		menue_main(MAINMENUE_READY);
	}

	switch ( REP_MODE ) {
	case REP_SLOTS:
		menue_slots();
		break;
	case REP_DISABLED:
		menue_main(MAINMENUE_OFF);
		if ( io_keys & 0x01 ) {
			REP_MODE = REP_SLOTS;
			break;
		}
		if ( io_keys & 0x02 )
			conf.mode = REP_READY;
		if ( (io_keys == 0x00) && (REP_MODE != conf.mode) ) {
			REP_MODE = conf.mode;
			config_save();
			cg_enable(1);
		}
		break;
	case REP_FREE:
		dispense_flag |= DISPENSE_FREE;
	case REP_READY:
		if ( i2c_keys && i2c_keys != last_keys )
			TASKS |= TASK_DISPENSE;
		last_keys = i2c_keys;
		if ( io_keys & 0x01 ) {
			REP_MODE = REP_SLOTS;
			break;
		}
		if ( io_keys & 0x02 )
			conf.mode = REP_DISABLED;
		if ( (io_keys == 0x00) && (REP_MODE != conf.mode) && (REP_MODE != REP_FREE) ) {
			REP_MODE = conf.mode;
			config_save();
			cg_enable(0);
		}
		if ( menue_timeout && (elapsed(&menue_time) <= menue_timeout) )
			return;
		menue_timeout = 0;
		menue_main(MAINMENUE_READY);
		break;
	case REP_SETUP:
		menue_setup();
		break;
	default:
		break;
	}

}

