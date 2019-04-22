/* Alex Chen (achen115@ucr.edu)
 * Nicholas Ray (nray001@ucr.edu)
 * achen115_lab4_part2.c
 *
 * Created: 4/10/2019 2:32:11 PM
 * Author : ucrcse
 */

#include <avr/io.h>



int main(void)
{
    /* Replace with your application code */
	DDRA = 0;	PINA = -1;
	DDRB = -1;	PORTB = 0;
	char B = 7;
	char state = 0;
    while (1) 
    {	
		char stateNext = ~PINA & 3;
		if(stateNext != state) {
			switch(stateNext) {
				case 0:
					break;
				case 1:
					//Make sure we're not getting here by releasing from state 3
					if(state != 3) {
						if(B < 9)
							B++;
					}
					break;
				case 2:
					//Make sure we're not getting here by releasing from state 3
					if(state != 3) {
						if(B > 0)
							B--;
					}
					break;
				case 3:
					B = 0;
					break;
			}
			state = stateNext;
		}
		PORTB = B;
    }
}

