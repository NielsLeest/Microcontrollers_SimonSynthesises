#include "spi.c"

void write_led_display( int value ) 
{
    int is_negative = value < 0;
    value *= (1 - is_negative * 2);
    int i = 1;
    do
    {
        spi_write_char(i, value % 10);
        value /= 10;
        i++;
    }
    while(i <= 4 && value != 0) ;
    if(is_negative)
        spi_write_char(i++, 0x0A);
    do
    {
        spi_write_char(i, 0x0F);
        i++;
    }
    while(i <= 4) ;
}

void led_display_clear()
{
	for (int i = 0; i < 4; i++)
	{
		spi_write_char(i, 0x0F);
	}
}