/*
 * achen115_lab1_part2.c
 *
 * Created: 4/3/2019 3:09:45 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    DDRA = DDRB = DDRC = 0; PINA = PINB = PINC = -1;
	DDRD = -1; PORTD = 0;
    while (1) 
    {
		unsigned char a = PINA, b = PINB, c = PINC;
		unsigned char total = a + b + c;
		unsigned char diff = a - c;
		PORTD = (total & 0xFC) | ((diff > 80 || -diff > 80) ? 2 : 0) | ((total > 140) ? 1 : 0); //Take the two lowest bits off of total
    }
}

