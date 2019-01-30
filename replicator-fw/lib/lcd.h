#define LCD_OFF             0x08   /* display off                            */
#define LCD_ON              0x0C   /* display on, cursor off                 */
#define LCD_ON_BLINK        0x0D   /* display on, cursor off, blink char     */
#define LCD_ON_CURSOR_OFF   0x0C   /* display on, cursor off                 */
#define LCD_ON_CURSOR_ON    0x0E   /* display on, cursor on                  */
#define LCD_ON_CURSOR_BLINK 0x0F   /* display on, cursor on, blink char      */
#define LCD_CLEAR           0x01   /* loescht die anzeige im lcd             */

#define LCD_CURSOR_HOME     0x02   /* move cursor 1,1                        */
#define LCD_CURSOR_LEFT     0x10   /* move cursor left  (decrement)          */
#define LCD_CURSOR_RIGHT    0x14   /* move cursor right (increment)          */
#define LCD_SHIFT_LEFT      0x18   /* shift display left                     */
#define LCD_SHIFT_RIGHT     0x1C   /* shift display right                    */

void lcd_init(void);
void lcd_send(char data);
void lcd_cmd(char cmd);
void lcd_char(char c);
void lcd_write_s(char* buf);
void lcd_home(void);
void lcd_clear(void);
void lcd_clearline(char line);
void lcd_on(void);
void lcd_off(void);
void lcd_off(void);
void lcd_pos(unsigned char row, unsigned char col);
void lcd_charDef(void);
