#include "lib/pcf8574.h"
#include "lib/lcd.h"

uint8_t i2c_error;

void pcf8574_init(void) {
	PORTC |=  (_BV(4) | _BV(5));
	// i2c clock = F_CPU / ( 16 + 2(TWBR) * (PRESCALER) )
	TWSR  |= _BV(TWPS0); // PRESCALER = 4
	TWBR   = 8; // 92.15kHz
}

uint8_t pcf8574_send_start(void) {
	/* writing a one to TWINT clears it, TWSTA=Start, TWEN=TWI-enable */
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	/* wait, until start condition has been sent --> ACK */
	while (!(TWCR & (1<<TWINT)));
	return TWSR & 0xF8;
}

void pcf8574_send_stop(void) {
	/* writing a one to TWINT clears it, TWSTO=Stop, TWEN=TWI-enable */
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);	
}

uint8_t pcf8574_send_addr_rw(uint8_t address, uint8_t rw) {
	/* address can be 0 .. 7; rw=0 --> write, rw=1 --> read */
	/* shift address one bit left */
	address <<= 1;

	/* set RW-Bit, if necessary */
	address |= rw;

	/* address of Expander
	    pcf8574  0b0100xxx0
	    pcf8574a 0b0111xxx0 */
	address |= 0b01110000;

	/* TWDR contains byte to send */
	TWDR = address;
	/* send content of TWDR */
	TWCR = (1<<TWINT) | (1<<TWEN);
	/* wait, until address has been sent --> ACK */
	while (!(TWCR & (1<<TWINT)));
	return TWSR & 0xF8;
}

uint8_t pcf8574_send_byte(uint8_t byte) {
	/* TWDR contains byte to send */
	TWDR = byte;
	/* send content of TWDR */
	TWCR = (1<<TWINT) | (1<<TWEN);
	/* wait, until byte has been sent --> ACK */
	while (!(TWCR & (1<<TWINT)));
	return TWSR & 0xF8;
}

uint8_t pcf8574_read_byte(void) {
	/* send content of TWDR; TWEA = enable ACK */
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	/* wait, until byte has been received --> ACK */
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

uint8_t pcf8574_get_inputs(uint8_t address) {
	uint8_t r;

	i2c_error = 0;
	r = pcf8574_send_start();
	if ( r != 0x08 && r != 0x10 ) {
		i2c_error = 1;
		return 0;
	}
	r = pcf8574_send_addr_rw(address, 1);
	if ( r != 0x40 ) {
		i2c_error = 2;
		return 0;
	}
	uint8_t input = pcf8574_read_byte();
	return input;
}

void pcf8574_set_outputs(uint8_t address, uint8_t byte) {
	uint8_t r;

	i2c_error = 0;
	r = pcf8574_send_start();
	if ( r != 0x08 && r != 0x10 ) {
		i2c_error = 1;
		return;
	}
	r = pcf8574_send_addr_rw(address, 0);
	if ( r != 0x18 ) {
		i2c_error = 2;
		return;
	}
	r = pcf8574_send_byte(byte);
	if ( r != 0x28 ) {
		i2c_error = 3;
		return;
	}
}

