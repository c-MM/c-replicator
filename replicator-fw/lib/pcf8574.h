#ifndef _PCF8574_H
#define _PCF8574_H


#include <avr/io.h>

extern uint8_t i2c_error;

void pcf8574_init (void);
void pcf8574_send_stop (void);
uint8_t pcf8574_send_start (void);
uint8_t pcf8574_send_addr_rw (uint8_t address, uint8_t rw);
uint8_t pcf8574_send_byte (uint8_t byte);
uint8_t pcf8574_read_byte (void);
extern uint8_t pcf8574_get_inputs (uint8_t address);
extern void pcf8574_set_outputs (uint8_t address, uint8_t byte);

#endif
