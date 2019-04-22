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
    /* Replace with your application code */
    DDRA = 0;	PINA = -1;
    DDRB = -1;	PORTB = 0;

	char states[255];
	for(int i = 0; i < 255; i++) {
		states[i] = i;
	}
	char stateIndex = 0;
	char pressed = 1;
    while (1)
    {
		if(~PINA & 1) {
			if(pressed) {
				
			} else {
				pressed = 1;
				if(stateIndex != 8) {
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

