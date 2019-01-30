/* vim: ts=4 ai fdm=marker
 *  
 *  copyright:  2010 - 2019 Martin Mueller (mm-at-sig21.net)
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
#include <avr/eeprom.h>
#include <util/atomic.h>

#define _CONFIG_C
#include "lib/config.h"

conf_t conf;

void config_init(void) {

	if (eeprom_read_byte(EE_VERSION) == CONF_VERSION) {
		eeprom_read_block(&conf, EE_OFFSET, sizeof(conf));
	} else {
		memcpy_P(&conf, &conf_def, sizeof(conf_t));
		config_save();
	}
}

void config_save(void) {
	eeprom_update_block(&conf, EE_OFFSET, sizeof(conf));
}

eecounter_t eecounter;

void counter_init(void) {

	if (eeprom_read_byte(EECOUNTER_OFFSET) == EECOUNTER_VERSION) {
		eeprom_read_block(&eecounter, EECOUNTER_OFFSET, sizeof(eecounter));
	} else {
		memset(&eecounter, 0, sizeof(eecounter));
		eecounter.version = EECOUNTER_VERSION;
		counter_save();
	}
}
	
void counter_save(void) {

	eeprom_update_block(&eecounter, (void*)(E2END + 1 - sizeof(eecounter)), sizeof(eecounter));
	while (! eeprom_is_ready() ) {}

}
