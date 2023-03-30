#define F_CPU 8e6
#define BIT(x) (1 << (x))

#include "ss_logic.c"

#include <avr/interrupt.h>

int main( void ) {
    sei(); // enable interrupts
	
	simon_says_init(3, 1, 1/6.0);

    while (1) {
        play_game();
    }

    return 1;
}