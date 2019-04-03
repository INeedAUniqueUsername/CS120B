/*
 * achen115_lab1_part1.c
 *
 * Created: 4/3/2019 2:33:56 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
    while (1) 
    {
		unsigned char open = PINA & 0x01, light = PINA & 0x02;
		PORTB = (open && !light) ? 1 : 0;
    }
	return 0;
}

