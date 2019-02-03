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

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "board.h"
#include "lib/timer1.h"
#include "lib/lcd.h"
#include "lib/pcf8574.h"
#include "lib/tasks.h"
#include "lib/menue.h"
#include "lib/changegiver.h"
#include "lib/config.h"
#include "lib/uart.h"

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void wdt_init(void) {
	MCUSR = 0;
	wdt_enable(WDTO_2S);
	return;
}

int main (void) {
	uint8_t last_tick = 0;

	CLKPR = 0x80;
	CLKPR = 0x00;

	pcf8574_init();
	pcf8574_set_outputs(I2C_RELAIS, 0);
	pcf8574_set_outputs(I2C_LEDS,   0);
	pcf8574_set_outputs(I2C_CRTL,   0xbf); // monoflop reset on

	// switch on pullups for service buttons
	PORTB |= (_BV(PB3) | _BV(PB4));

	config_init();
	if ( PINB & _BV(PB4) )
		REP_MODE = conf.mode;
	else
		REP_MODE = REP_SETUP;

	cg_init();


	lcd_init();
	snprintf_P(disp_line1, LCD_BUF, PSTR("Version:         "));
	snprintf_P(disp_line2, LCD_BUF, PSTR("%s               "), VERSION);
	lcd_pos(0,0);
	lcd_write_s(disp_line1);
	lcd_pos(1,0);
	lcd_write_s(disp_line2);
	_delay_ms(1000);

	counter_init();

	timer1_init();

	uart_init();

	sei();

	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();

	if ( REP_MODE == REP_READY )
		cg_enable(1);
	else
		cg_enable(0);

	leds[8] = LED_ON;

	TASKS |= TASK_DISPLAY;
	TASKS |= TASK_MOTPOS;
	TASKS |= TASK_SLOTS;
	TASKS |= TASK_LEDS;

	handle_tasks();
	last_keys = i2c_keys;

	while (1) {
		wdt_reset();

		if ( ticks != last_tick ) {
			if (ticks % 5 == 0)
				TASKS |= TASK_KEYS;

			if (ticks % 5 == 1)
				TASKS |= TASK_LEDS;

			if (ticks % 5 == 3)
				TASKS |= TASK_SERIALRX;

			if (ticks % 25 == 23)
				TASKS |= TASK_MOTPOS;

			if (ticks % 25 == 24)
				TASKS |= TASK_SLOTS;

			if (ticks % 100 == 99)
				TASKS |= TASK_SETLEDS;

			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				if ( ticks >= 200 )
					ticks = 0;
			}
			last_tick = ticks;
		}

		handle_tasks();

		sleep_cpu();
	}
}
