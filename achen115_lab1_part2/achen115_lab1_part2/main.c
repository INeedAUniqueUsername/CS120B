/*
 * achen115_lab1_part2.c
 *
 * Created: 4/3/2019 3:09:45 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    DDRA = 0; PINA = -1;
	DDRC = -1; PORTC = 0;
    while (1) 
    {
		unsigned char a = PINA,
			c = !(PINA & 1) + !(PINA & 2) + !(PINA & 4) + !(PINA & 8) | ((PINA & 0x0F) == 0x0F ? 0x80 : 0);
		PORTC = c;
    }
}

