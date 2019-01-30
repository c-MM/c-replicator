/*
Eine kleine Funktionssammlung fuer den Zugriff auf das LCD von myAVR.
Urspruenglich von Peter Fleury entwickelt, da R/W nur auf LOW und somit
kein Bussyabfrage und keine Adresscounterabfrage moeglich ist von Manuel
Buehrer umgeschrieben und letztendlich von mir (Roman Mainer) aufgeraeumt und
in kleinen Teilen umgeschrieben und ein Beispiel hinzugefuegt.
*/

#include "lcd.h"

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

/* 
 * lcd_out() 
 * sends one byte to the controler respecting the current
 * setting of RS (the bit switshing between command and data mode)
 */
void lcd_out(char data) {
	char rs, tmp;

	rs = PORTD;		/* get current RS setting */
	rs &= 0x04;
	
	tmp = data;		/* send 4 upper bits */
	tmp &= 0xf0;
	tmp |= rs;
	PORTD = tmp;
	PORTD |= (1<<3); PORTD &= ~(1<<3);
	_delay_us(10);

	tmp = data;		/* send 4 lower bits */
	tmp &= 0x0f;
	tmp *= 0x10;
	tmp |= rs;
	PORTD = tmp;
	PORTD |= (1<<3); PORTD &= ~(1<<3);

	_delay_us(50);
}

/*
 * lcd_cmd()
 * calls lcd_out() with RS deasserted to signal command mode
 */
void lcd_cmd(char cmd) {
	PORTD &= ~(1<<2);	/* clear RS */
	lcd_out(cmd);
}

/*
 * lcd_char()
 * calls lcd_out() with RS asserted to signal data mode
 */
void lcd_char(char c) {
	PORTD |= (1<<2);	/* set RS */
	lcd_out(c);
}

/*
 * lcd_write_s(..) - sendet eine Zeichenkette an LCD
 */
void lcd_write_s(char *buf)
{
	while(buf[0]!='\0') {
		lcd_char(buf[0]);
		buf++;
	}
}

/*
 * lcd_clear(..) - loescht die Anzeige im LCD
 */
void lcd_clear(void)
{
	lcd_cmd(LCD_CLEAR);
	_delay_ms(2);
}

/*
 * lcd_on(..) - schaltet das LCD und den Cursor an
 */
void lcd_on(void)
{
	lcd_cmd(LCD_ON_CURSOR_ON);
}

/*
 * lcd_off(..) - schaltet das LCD aus
 */
void lcd_off(void)
{
	lcd_cmd(LCD_OFF);
}

/*
 * lcd_pos()
 * 	set cursor position, 0,0 is upper left corner
 */
void lcd_pos(unsigned char row, unsigned char col) {
	char tmp;

	row &= 0x01;		/* clear upper 7 bits, the display has only 2 lines */
	row *= 0x40;		/* shift line to command bit 6 */
	col &= 0x0f;		/* clear upper 4 bits of column */
	tmp = row|col;		/* combine row and column values */
	tmp |= 0x80;		/* add "set cursor" command */
	lcd_cmd(tmp);
}

void lcd_clearline(char line) {
	lcd_pos(line, 0);
	lcd_write_s("                ");
	lcd_pos(line, 0);
}

/*
 * lcd_init()
 * 	initialize myAVR LCD board ports, boot controler
 *	and set 4-bit and infix mode
 */
void lcd_init(void)
{
	DDRD  = 0xfc;
	PORTD = 0x00;
	_delay_ms(65);	/* wait for controller to boot */

	/* 
	 * Softreset: send 3 times 0x30
	 */
	PORTD = 0x30;
	PORTD |= (1<<3); PORTD &= ~(1<<3);
	_delay_ms(5);

	PORTD = 0x30;
	PORTD |= (1<<3); PORTD &= ~(1<<3);
	_delay_us(100);

	PORTD = 0x30;
	PORTD |= (1<<3); _delay_us(1); PORTD &= ~(1<<3);
	_delay_us(50);

	PORTD = 0x20;		/* switch display in 4-bit mode */
	PORTD |= (1<<3); _delay_us(1); PORTD &= ~(1<<3);
	_delay_ms(5);

	lcd_cmd(0x28);		/* set display 2 lines, 5x7 matrix */
	lcd_off();
	lcd_clear();
	lcd_cmd(0x06);		/* set infix mode */
	lcd_on();
	lcd_cmd(LCD_ON_CURSOR_OFF);
}

/*
 * lcd_charDef(..) - initialisiert benutzerdefinierte Zeichen für das LCD
 */
void lcd_charDef(void)
{
	// 1. Kommando CGRAM + Adresse setzen
	// 2. Punktmatrix-Daten laden
	unsigned char charSet[64]={	
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
		0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,
		0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,
		0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
		0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
		0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	};
	int i=0;
	for (i=0;i<64;i++) {
		lcd_cmd(0x40+i);	// cmd 0x40 + Adr
		lcd_char(charSet[i]);	// Pixel
	}
}

