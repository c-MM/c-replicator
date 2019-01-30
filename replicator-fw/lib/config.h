/* vim: ts=4 ai fdm=marker
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

#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#define CONF_VERSION	0x01

#define conf_def	_conf[0]
#define conf_min	_conf[1]
#define conf_max	_conf[2]

typedef struct {
	uint8_t		version;		// 0x00 config version
	uint8_t		slots;			// 0x01 number of slots
	uint8_t		slot_state;		// 0x02	operation state slots
	uint8_t		mode;			// 0x03 operation mode of replicator
	uint8_t		price[4];		// 0x04 the 4 prices of the changegiver in cent/10
	uint8_t		pr_alien[8];	// 0x08 price of slots for aliens, values: 0-3
	uint8_t		pr_member[8];	// 0x10 price of slots for members, values: 0-3
} conf_t;

extern conf_t conf;
extern const conf_t _conf[3];

void config_init(void);
void config_save(void);

typedef struct {
	uint8_t		version;
	uint16_t	alien[8];
	uint16_t	member[8];
	uint16_t	casse[8];
	uint16_t	error[8];
} eecounter_t;

extern eecounter_t eecounter;
void counter_init(void);
void counter_save(void);


// declare default settings for eeprom if included from config.c
#ifdef _CONFIG_C

#define EE_OFFSET	(void*)4	// leave the first 4 bytes unused
#define EE_VERSION	(void*)4	// this is the first byte of the stored configuration

#define EECOUNTER_OFFSET	(void*)(E2END + 1 - sizeof(eecounter))
#define EECOUNTER_VERSION	0x01

const conf_t PROGMEM _conf[3] = {
//			default value					min value				max value
/* 0x04 */	[0].version		= CONF_VERSION,	[1].version		= 0x0,	[2].version		= 0xff,
/* 0x05 */	[0].slots		= 0x7,			[1].slots		= 0x0,	[2].slots		= 0xff,
/* 0x06 */	[0].slot_state	= 0x00,			[1].slot_state	= 0x0,	[2].slot_state	= 0xff,
/* 0x07 */	[0].mode		= 0x03,			[1].mode		= 0x0,	[2].mode		= 0xff,
/* 0x08 */	[0].price = { 10, 13, 20, 25 },	[1].price = {0,0,0,0},	[2].price = {50,50,50,50},
/* 0x10 */	[0].pr_alien  = { 3, 3, 3, 3, 3, 3, 3, 3},
			[1].pr_alien  = { 1, 1, 1, 1, 1, 1, 1, 1},
			[2].pr_alien  = { 4, 4, 4, 4, 4, 4, 4, 4},
/* 0x18 */	[0].pr_member = { 2, 2, 2, 2, 2, 2, 2, 2},
			[1].pr_member = { 1, 1, 1, 1, 1, 1, 1, 1},
			[2].pr_member = { 4, 4, 4, 4, 4, 4, 4, 4},
};

#endif
#endif
