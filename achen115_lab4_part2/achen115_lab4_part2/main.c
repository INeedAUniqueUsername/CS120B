/*
 * achen115_lab4_part2.c
 *
 * Created: 4/10/2019 2:17:46 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRA = 0;	PINA = -1;
	DDRC = -1;	PORTC = 0;
	char C = 7;
    while (1) 
    {
		if((PINA & 3) == 3) {
			C = 0;
		} else if(PINA & 2) {
			if(C > 0)
				C--;
		} else if(PINA & 1) {
			if(C < 10)
				C++;
		}
		PORTC = C;
    }
}

