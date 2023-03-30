#include "ss_part.c"
#include "adc.c"
#include "lcd.c"
#include "ssd_leds.c"

#include <avr/interrupt.h>
#include <math.h>

//#define BIT(x) (1 << (x))

int score;
int high_score;
int lives;
static unsigned char max_lives;
static unsigned char amount_of_octaves;
static float max_difference;
ss_part* main_part = NULL;

static int game_seed = 0;

static unsigned int pulses = 0;
void set_frequency(float frequency)
{
	unsigned int interval = round(1e6 / frequency);
	
	ICR1 = interval;
	OCR1A = interval/2;// Compare value of counter 2
}

void button_init() 
{
	DDRD = 0x00;

    EICRA |= 0x02;            // falling edge
    EIMSK |= 0x01;
}

void timer1_init( void ) 
{
	
	int interval = 1;
	
	DDRB |= (1<<PD5);
	
	ICR1 = interval;
	OCR1A = interval;// Compare value of counter 2
	TIMSK |= BIT(7);// T2 compare match interrupt enable
	TCCR1A = 0b10000010;
	TCCR1B = 0b00011010;
}

void timer2_init( void ) 
{
	OCR2 = 1000; 
    TIMSK |= BIT(7); 
    TCCR2 = 0b00001010;
}

ISR(TIMER2_COMP_vect) {
	pulses++;
	pulses %= 4096;
}

static int button_trigger = 0;
ISR(INT0_vect)
{
	button_trigger = 1;
}

int get_button_trigger()
{
	if (button_trigger)
	{
		button_trigger = 0;
		return 1;
	}
	return 0;
}

void reset_button_trigger()
{
	button_trigger = 0;
}

void simon_says_init(unsigned char max_lives_init, unsigned char amount_of_octaves_init, float max_difference_init){
    max_lives = max_lives_init;
    amount_of_octaves = amount_of_octaves_init;
    max_difference = max_difference_init;
    score = high_score;
    lives = max_lives;
	
	lcd_init();
	button_init();
	timer1_init();
	timer2_init();
	adc_init();
    spi_init();
}

void start_screen()
{
	
}

float byte_to_frequency(unsigned char value)
{
	float octave_interval = value / (256.0 / amount_of_octaves);
	float frequency = 440 * pow(2, octave_interval);
	return frequency;
}

void play_from_adc(){
    float frequency =  byte_to_frequency(read_adc());
    set_frequency(frequency);
    _delay_ms(10);
}
void play_from_list(unsigned char value){
    float frequency = byte_to_frequency(value);
    set_frequency(frequency);
    _delay_ms(1000);
    set_frequency(0);
}

int check_difference_from_adc(unsigned char value){
    unsigned char adc_value = read_adc();
    float octave_interval_adc = adc_value / (256.0 / amount_of_octaves);
    float octave_interval_value = value / (256.0 / amount_of_octaves);
    if (octave_interval_adc < octave_interval_value - max_difference ||  octave_interval_adc > octave_interval_value + max_difference){
        return 0;
    } return 1;
}

void print_lives_and_score(){
    PORTA = (1 << lives) - 1;
	lcd_clear();
	display_text("Score: ");
	display_text(to_string(score));
	set_cursor(17);
	display_text("Highscore: ");
	display_text(to_string(high_score));
}

void play_game(){
	write_led_display(high_score);
	lcd_clear();
	display_text("SimonSynthesises");
	set_cursor(16);
	display_text("Press to start!");
	
	reset_button_trigger();
	while(!get_button_trigger());
    init_game_seed(pulses);
    lives = max_lives;
    while(lives > 0){
        main_part = generate_new_part(main_part);
        ss_part* current_part = main_part;
        print_lives_and_score();
        int current = 0;
        while(current_part != NULL){
            write_led_display(--current);
            play_from_list(current_part->value);
            _delay_ms(100);
			current_part = current_part->next;
        }
		led_display_clear();
		_delay_ms(1000);
        current = 0;
        current_part = main_part;
        while(current_part != NULL){
            write_led_display(++current);
			reset_button_trigger();
            while(!get_button_trigger()){
               play_from_adc();
            }
            if (check_difference_from_adc(current_part->value)){
                current_part = current_part->next;
                score++;
                if (score > high_score){
                    high_score = score;
                }
                print_lives_and_score();
            } else {
                lives--;
                break;
            }
            _delay_ms(100);
        }
		led_display_clear();
		_delay_ms(1000);
    }
	led_display_clear();
    print_lives_and_score();
	score = 0;
    delete_part(main_part);
    main_part = NULL;
	set_frequency(0);
	_delay_ms(1000);
}

