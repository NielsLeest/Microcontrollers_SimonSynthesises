#define BIT(a) (1 << (a))
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

void adc_init() {
	ADMUX = 0b01100011;
	ADCSRA = 0b11000110;
	
	DDRF = 0x00;
	DDRA = 0xFF;
}

unsigned char read_adc(){
    unsigned char adc_value = ADCH;
	ADCSRA |= BIT(6);
    return adc_value;
}