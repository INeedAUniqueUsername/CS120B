/*
 * achen115_lab3_part4.c
 *
 * Created: 4/8/2019 2:55:39 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0;	PINA = -1;
	DDRB = -1;	PORTB = 0;
	DDRC = -1;	PORTC = 0;
    /* Replace with your application code */
    while (1) 
    {
		char A = PINA;
		char upper = A >> 4;
		PORTB = (PORTB & 0xF0) | upper;
		char lower = A << 4;
		PORTC = (PORTC & 0x0F) | lower;
    }
}

