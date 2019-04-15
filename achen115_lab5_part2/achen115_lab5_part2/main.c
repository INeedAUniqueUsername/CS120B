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
	DDRC = -1;	PORTC = 0;
	char C = 7;
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
						if(C < 9)
							C++;
					}
					break;
				case 2:
					//Make sure we're not getting here by releasing from state 3
					if(state != 3) {
						if(C > 0)
							C--;
					}
					break;
				case 3:
					C = 0;
					break;
			}
			state = stateNext;
		}
		PORTC = C;
    }
}

