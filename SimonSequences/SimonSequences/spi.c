
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define BIT(x) (1 << (x))

#define DDR_SPI		DDRB					// spi Data direction register
#define PORT_SPI	PORTB					// spi Output register
#define SPI_SCK		1						// PB1: spi Pin System Clock
#define SPI_MOSI	2						// PB2: spi Pin MOSI
#define SPI_MISO	3						// PB3: spi Pin MISO
#define SPI_SS		0						// PB0: spi Pin Slave Select

void spi_master_init(void)
{
	DDR_SPI = 0xff;							// All pins output: MOSI, SCK, SS, SS_display as output
	DDR_SPI &= ~BIT(SPI_MISO);				// 	except: MISO input
	PORT_SPI |= BIT(SPI_SS);				// SS_ADC == 1: deselect slave
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);	// or: SPCR = 0b11010010;
											// Enable spi, MasterMode, Clock rate fck/64, bitrate=125kHz
											// Mode = 0: CPOL=0, CPPH=0;
}


// Write a byte from master to slave
void spi_write( unsigned char data )				
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete 
}

// Write a byte from master to slave and read a byte from slave
// nice to have; not used here
char spi_write_read( unsigned char data )
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete 
	data = SPDR;							// New received data (eventually, MISO) in SPDR
	return data;							// Return received byte
}

// Select device on pinnumer PORTB
void spi_slave_select(unsigned char chipNumber)
{
	PORT_SPI &= ~BIT(chipNumber);
}

// Deselect device on pinnumer PORTB
void spi_slave_deselect(unsigned char chipNumber)
{

	PORT_SPI |= BIT(chipNumber);
}


// Initialize the driver chip (type MAX 7219)
void display_driver_init() 
{
	spi_slave_select(0);				// Select display chip (MAX7219)
  	spi_write(0x09);      			// Register 09: Decode Mode
  	spi_write(0xFF);				// 	-> 1's = BCD mode for all digits
  	spi_slave_deselect(0);			// Deselect display chip

  	spi_slave_select(0);				// Select dispaly chip
  	spi_write(0x0A);      			// Register 0A: Intensity
  	spi_write(0x01);    			//  -> Level 1 (in range [1..F])
  	spi_slave_deselect(0);			// Deselect display chip

  	spi_slave_select(0);				// Select display chip
  	spi_write(0x0B);  				// Register 0B: Scan-limit
  	spi_write(0x03);   				// 	-> 1 = Display digits 0..3
  	spi_slave_deselect(0);			// Deselect display chip

  	spi_slave_select(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x01); 				// 	-> 1 = Normal operation
  	spi_slave_deselect(0);			// Deselect display chip
}

void display_on() 
{
  	spi_slave_select(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x01); 				// 	-> 1 = Normal operation
  	spi_slave_deselect(0);			// Deselect display chip
}

void display_off() 
{
  	spi_slave_select(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x00); 				// 	-> 1 = Normal operation
  	spi_slave_deselect(0);			// Deselect display chip
}

void spi_init()
{
	DDR_SPI=0x01;					  	// Set PB0 pin as output for display select
	spi_master_init();              	// Initialize spi module
	display_driver_init();            // Initialize display chip
}

void spi_write_char ( unsigned char address, unsigned char data ) 
{
    spi_slave_select(0);                // Select display chip
    spi_write(address);             
    spi_write(data);                 
    spi_slave_deselect(0);            // Deselect display chip
}