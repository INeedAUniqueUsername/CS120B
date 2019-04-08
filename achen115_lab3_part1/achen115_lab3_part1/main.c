/*
 * achen115_lab3_part1.c
 *
 * Created: 4/8/2019 2:34:55 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
    DDRA = 0;	PINA = -1;
    DDRB = 0;	PINB = -1;
    DDRC = -1;	PORTC = 0;
    while (1)
    {
	    char count = 0;
	    char A = PINA;
	    while(A > 0) {
		    if(A%2 == 1)
		    count++;
		    A >>= 1;
	    }
	    char B = PINB;
	    while(B > 0) {
		    if(B%2 == 1)
		    count++;
		    B >>= 1;
	    }
	    PORTC = count;
    }
}

