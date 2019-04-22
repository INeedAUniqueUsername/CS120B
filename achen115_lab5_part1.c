/*
 * achen115_lab3_part2.c
 *
 * Created: 4/8/2019 2:25:57 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRA = 0;	PINA = -1;
	DDRC = -1;	PORTC = 0;
    while (1) 
    {
		char fuel = ~(PINA | 0xF0);
		PORTB = ((fuel < 5) << 6) | ((fuel > 0) << 5) | ((fuel > 2) << 4) | ((fuel > 4) << 3) | ((fuel > 6) << 2) | ((fuel > 9) << 1) | (fuel > 12);
    }
}

