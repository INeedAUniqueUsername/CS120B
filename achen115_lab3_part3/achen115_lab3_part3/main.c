/*
 * achen115_lab3_part3.c
 *
 * Created: 4/8/2019 2:46:59 PM
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
		char A = PINA;
	    char fuel = A & 0x0F;
	    PORTC = (((A & 0x70) == 0x30) << 7) |		//A == X011 XXXX
				((fuel < 5) << 6) |
				((fuel > 0) << 5) |
				((fuel > 2) << 4) |
				((fuel > 4) << 3) |
				((fuel > 6) << 2) |
				((fuel > 9) << 1) |
				(fuel > 12);
    }
}

