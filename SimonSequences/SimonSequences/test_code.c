#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "lcd.c"

#define BIT(a) (1 << (a))



// wait(): busy waiting for 'ms' millisecond
// used library: util/delay.h
void wait(int ms)
{
	for (int i=0; i<ms; i++)
		{
			_delay_ms(1);
		}
}




void lcd_command( unsigned char data ){
pin_rs(0);
PORTC = data;
pin_e(1);
pin_e(0);
}










ISR( TIMER1_COMPA_vect ) {
	
}



int main( void ) {
	DDRC = 0xFF;
	
	adc_init();
	timer1_init();
	spi_init();
	
	write_led_display(69);
	
	sei(); // turn_on intr all
	
	//set_frequency(440);

	while (1) {

		
		
		set_frequency(byte_to_frequency(adc_value));
		_delay_ms(10);
	}

	return 1;
}





