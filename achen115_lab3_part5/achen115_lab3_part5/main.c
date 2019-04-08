/*
 * achen115_lab3_part5.c
 *
 * Created: 4/8/2019 3:01:34 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRB = 0;	PINB = -1;
	DDRD = 0;	PIND = -1;
    while (1) 
    {
		long weight = (PIND << 1) + PINB0;
		//Clear the bits
		char B = PORTB & 0xF9;;
		if(weight > 5) {
			if(weight < 70) {
				B |= 1 << 2;
			} else {
				B |= 1 << 1;
			}
		}
		PORTB = B;
    }
}

