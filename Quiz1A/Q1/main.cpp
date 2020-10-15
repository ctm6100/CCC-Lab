/*
 * Quiz1AQ1.cpp
 *
 * Created: 10/14/2020 10:15:52 AM
 * Author : *************
 * 
 */ 
#include <avr/io.h>

int main(void)
{
	int state =0;
	/* Replace with your application code */
	DDRC = 0xFF;//output
	PORTC = 0x01;
	
	DDRD = 0x00;
	PORTD = 0xFF;

	TCCR1A = 0x00; //output clock source
	TCCR1B = 0x0F; //rising edge
	OCR1A = 4; //0->1->2->3->4(clear timer)(new loop)0
	while (1)
	{
		do {
			if (TCNT1 == 0) {
				PORTC = 0x01;
				}else if (TCNT1 == 1){
				PORTC = 0x02;
				}else if (TCNT1 == 2){
				PORTC = 0x03;
				}else if (TCNT1 == 3){
				PORTC = 0x00;
			}
			
		}while ((TIFR1 & (0x1<<OCF1A)) == 0); // wait for TOV1
		TIFR1 = 0x1<<OCF1A; //clear TOV1


	}
}
