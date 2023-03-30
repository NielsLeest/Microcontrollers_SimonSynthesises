#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define LCD_E     3
#define LCD_RS    2

char * to_string(int value) {
	char *str = malloc(16 * sizeof(char));
	itoa(value, str, 10);
	return &(str[0]);
}



void lcd_e(void) {
	PORTC |= (1<<LCD_E);    // E high
	_delay_ms(1);
	PORTC &= ~(1<<LCD_E);   // E low
	_delay_ms(1);
}

void display_text(char *str){
	for(;*str; str++){
		lcd_write(*str);
	}
}

void set_cursor(int position){
	lcd_write_cmd(0x80 | (position & 0x0F) | ((position << 2) & 0x40));
}

void lcd_init(){
	DDRC = 0xFF;
	PORTC = 0x00;
	PORTC = 0x20;
	lcd_e();
	PORTC = 0x20;
	lcd_e();
	PORTC = 0x80;
	lcd_e();
	PORTC = 0x00;
	lcd_e();
	PORTC = 0xF0;
	lcd_e();
	PORTC = 0x00;
	lcd_e();
	PORTC = 0x60;
	lcd_e();
	
	lcd_clear();
	
	set_cursor(0);
}

void lcd_write(unsigned char byte) {
	// First nibble.
	PORTC = byte;
	PORTC |= (1<<LCD_RS);
	lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC |= (1<<LCD_RS);
	lcd_e();
}

void lcd_write_cmd(unsigned char byte)

{
	// First nibble.
	PORTC = byte;
	PORTC &= ~(1<<LCD_RS);
	lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC &= ~(1<<LCD_RS);
	lcd_e();
}

void lcd_clear() {
	lcd_write_cmd (0x01);                        //Leeg display
	_delay_ms(2);
	lcd_write_cmd (0x80);                        //Cursor terug naar start
}