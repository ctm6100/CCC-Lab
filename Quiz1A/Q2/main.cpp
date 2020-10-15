/*
 * Quiz1AQ2.cpp
 *
 * Created: 10/14/2020 10:36:15 AM
 * Author : *****************
 */ 

#include <avr/io.h>

void T0Delay(){ //8000us, 125times = 1s
	OCR0A = 124;//125-1
	TCCR0A = 0x02;//set to CTC mode
	TCCR0B = 0x05;//prescaler 1024
	while ((TIFR0&(1<<OCF0A))==0);
	TCCR0A=0;
	TCCR0B=0;
	TIFR0 = 0x02;//clear

}

void D2s(){ //8000us, 250times = 2s
	for (int i=0; i <= 249; i++){
		T0Delay();
	}

}
void D1h5s(){ //8000us, 187.5times = 1.5s
	for (int i=0; i <= 187; i++){
		T0Delay();
	}

}

void D1s(){ //8000us, 125times = 1s
	for (int i=0; i <= 124; i++){
		T0Delay();
	}

}
void Dh5s(){ //8000us, 62.5times = 0.5s
	for (int i=0; i <= 62; i++){
		T0Delay();
	}

}

int main(void)
{
	int state =0;
	/* Replace with your application code */
	DDRC = 0xFF;//output
	PORTC = 0x01;
	
	DDRD = 0x00;
	PORTD = 0xFF;

	TCCR1A = 0x00; //output clock source
	TCCR1B = 0x0E; //output clock source
	OCR1A = 4;//0->1->2->3->4(clear timer)(new loop)0
	while (1)
	{
		do {
			if (TCNT1 == 0) {
				D2s();
				PORTC ^= 0x03;//flash
				}else if (TCNT1 == 1){
				D1h5s();
				PORTC ^= 0x03;
				}else if (TCNT1 == 2){
				D1s();
				PORTC ^= 0x03;
				}else if (TCNT1 == 3){
				Dh5s();
				PORTC ^= 0x03;
			}
			
		}while ((TIFR1 & (0x1<<OCF1A)) == 0); // wait for TOV1
		TIFR1 = 0x1<<OCF1A; //clear TOV1


	}
}

