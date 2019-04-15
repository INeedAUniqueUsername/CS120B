/*
 * achen115_lab5_part3.c
 *
 * Created: 4/15/2019 2:58:13 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include<stdlib.h>

int main(void)
{
	srand(time(0));
    /* Replace with your application code */
    DDRA = 0;	PINA = -1;
    DDRB = -1;	PORTB = 0;
	char states[65];
	for(int i = 0; i < 63; i++) {
		states[i] = rand()%64;
	}
	states[64] = 0;
	char stateIndex = 0;
	char pressed = 1;
    while (1)
    {
		if(~PINA & 1) {
			if(pressed) {
				
			} else {
				pressed = 1;
				if(states[stateIndex]) {
					stateIndex++;
				} else {
					stateIndex = 0;
				}
			}
		} else {
			pressed = 0;
		}
		
		PORTB = states[stateIndex];
    }
}

