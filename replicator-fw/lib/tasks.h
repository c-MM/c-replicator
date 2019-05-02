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

#ifndef _TASKS_H
#define _TASKS_H

#include <stdint.h>

typedef enum {
    REP_DISABLED,
	REP_READY,
	REP_FREE,
	REP_DEBUG,
	REP_SETUP,
	REP_SLOTS,
} replicator_mode_t;

typedef enum {
	NONE,
	STARTING,
	STARTED,
	RUNNING,
	FINISHED,
	ERROR,
	DENY,
} dispense_state_t;

extern uint8_t last_keys, key_valid, io_keys, i2c_keys, i2c_slots, i2c_motpos, dispense_flag;
extern volatile uint16_t TASKS;
extern replicator_mode_t REP_MODE;
extern uint8_t leds[10];

#define TASK_KEYS		(1<<0)
#define TASK_LEDS		(1<<1)
#define TASK_DISPLAY	(1<<2)
#define TASK_MOTPOS		(1<<3)
#define TASK_SLOTS		(1<<4)
#define TASK_SETLEDS	(1<<5)
#define TASK_DISPENSE	(1<<6)
#define TASK_SERIALRX	(1<<7)

#define DISPENSE_FREE		0x01
#define DISPENSE_MEMBER		0x02
#define DISPENSE_FORCE		0x04

void handle_tasks (void);
void start_dispense (void);
int8_t slot_selected (void);

#endif /* TASKS_H */
