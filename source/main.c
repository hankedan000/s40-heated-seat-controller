/*
 * s40_heated_seats.c
 *
 * Created: 11/17/2015 9:12:42 PM
 * Author : Daniel
 */ 
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* INDICATORS */
#define IND_TRIS		DDRB
#define IND_PORT		PORTB
#define IND_PIN			PINB
#define DHI_IND			PORTB0
#define DLO_IND			PORTB1
#define PHI_IND			PORTB2
#define PLO_IND			PORTB3
/* BUTTON */
#define BUTTON_TRIS		DDRB
#define BUTTON_PORT		PORTB
#define BUTTON_PIN		PINB
#define BUTTON			PORTB4
/* FET DRIVERS */
#define FET_TRIS		DDRA
#define FET_PORT		PORTA
#define FET_PIN			PINA
#define DLO_FET			PORTA1
#define DHI_FET			PORTA2
#define PHI_FET			PORTA3
#define PLO_FET			PORTA4

/* I/O MANIPULATORS */
#define PHI				IND_PORT |= (1<<PHI_IND); IND_PORT &= ~(1<<PLO_IND); FET_PORT |= (1<<PHI_FET); FET_PORT &= ~(1<<PLO_FET)
#define PLOW			IND_PORT |= (1<<PLO_IND); IND_PORT &= ~(1<<PHI_IND); FET_PORT &= ~(1<<PHI_FET); FET_PORT |= (1<<PLO_FET)
#define POFF			IND_PORT &= ~(1<<PLO_IND); IND_PORT &= ~(1<<PHI_IND); FET_PORT &= ~(1<<PHI_FET); FET_PORT &= ~(1<<PLO_FET)
#define DHI				IND_PORT |= (1<<DHI_IND); IND_PORT &= ~(1<<DLO_IND); FET_PORT |= (1<<DHI_FET); FET_PORT &= ~(1<<DLO_FET)
#define DLOW			IND_PORT |= (1<<DLO_IND); IND_PORT &= ~(1<<DHI_IND); FET_PORT &= ~(1<<DHI_FET); FET_PORT |= (1<<DLO_FET)
#define DOFF			IND_PORT &= ~(1<<DLO_IND); IND_PORT &= ~(1<<DHI_IND); FET_PORT &= ~(1<<DHI_FET); FET_PORT &= ~(1<<DLO_FET)
#define BUTTON_VAL		!!(BUTTON_PIN&(1<<BUTTON))

//---------------- FUNCTION PROTOTYPES -----------------+
// Function prototypes for main							|
//------------------------------------------------------+

int main(void) {
	//------------------------ SETUP -----------------------+
	// Initialize and setup all start variables				|
	//------------------------------------------------------+
	/* PORT SETUP */
	IND_TRIS |= (1<<PHI_IND)|(1<<PLO_IND)|(1<<DHI_IND)|(1<<DLO_IND);// OUTPUTS
	FET_TRIS |= (1<<PHI_FET)|(1<<PLO_FET)|(1<<DHI_FET)|(1<<DLO_FET);// OUTPUTS
	BUTTON_TRIS &= ~(1<<BUTTON);// INPUTS
	BUTTON_PORT |= (1<<BUTTON);// enable pull-ups on button
	
	
	/* INTERRUPT SETUP */
	TCCR0A	|= 0x02;
	TCCR0B	|= 0x02; // x1 prescaler
	OCR0A	= F_CPU/8/19200; // set timer compare A for period of 1bit at 19200baud
	
	/* PRE LAUNCH */
	_delay_ms(1000);// wait for power to stabilize
	uint8_t i = eeprom_read_byte((uint8_t*)0);// grab last state code before power down


	//------------------------ LOOP ------------------------+
	// Initialize and setup all start variables				|
	//------------------------------------------------------+
	while(1){
		if(BUTTON_VAL){
			if(++i>=7)	i=0;// increment state
			eeprom_write_byte((uint8_t*)0, i);
			while(BUTTON_VAL);
		}// if
		_delay_ms(100);
		
		switch(i){
			case 0:
			DOFF;	POFF;
			break;
			case 1:
			DHI;	POFF;
			break;
			case 2:
			DLOW;	POFF;
			break;
			case 3:
			DHI;	PHI;
			break;
			case 4:
			DLOW;	PLOW;
			break;
			case 5:
			DOFF;	PHI;
			break;
			case 6:
			DOFF;	PLOW;
			break;
			default:
			DOFF;	POFF;
		}// switch
	}// while
	return 0;
}

