/*
 * q1.cpp
 *
 * Created: 
 * Author : 
 */ 

#include <avr/io.h>
#include "avr/interrupt.h"
#include <ctype.h>

void LEDa1on(){
	PORTB |= (1 << 0);
}
void LEDa1off(){
	PORTB &= ~(1 << 0);
}
void LEDa1Toggle(){
	PORTB ^= (1 << 0);
}

void LEDa2on(){
	PORTB |= (1 << 1);
}
void LEDa2off(){
	PORTB &= ~(1 << 1);
}
void LEDa2Toggle(){
	PORTB ^= (1 << 1);
}


void LEDb1on(){
	PORTC |= (1 << 4);
}

void LEDb1off(){
	PORTC &= ~(1 << 4);
}
void LEDb1Toggle(){
	PORTC ^= (1 << 4);
}

void LEDb2on(){
	PORTC |= (1 << 5);
}
void LEDb2off(){
	PORTC &= ~(1 << 5);
}
void LEDb2Toggle(){
	PORTC ^= (1 << 5);
}


void LEDsetup(){
	
	DDRC |= (1 << 4);
	DDRC |= (1 << 5);
	
	DDRB |= (1 << 0);
	DDRB |= (1 << 1);
	
	//LEDa1on();
	//LEDa2on();
	
	//LEDb1on();
	//LEDb2on();
	
}

//Timer2 ===================================================
//Timer2 Output Compare Match A Interrupt Enable
void T2MatchInterruptEnable(){
	TIMSK2 |= (1 << OCIE2A);
	//sei();
}

//Timer2 Output Compare Match A Interrupt Disable
void T2MatchInterruptDisable(){
	TIMSK2 &= ~(0 << OCIE2A);
}

void T2DelayInt(){ //each CTC match = 8000us, for interupt
	OCR2A = 124;//125-1
	TCCR2A = 0x02;//set to CTC mode
	TCCR2B = 0x07;//prescaler 1024
}

void T2Delay(){ //8000us, 125times = 1s
	OCR2A = 124;//125-1
	TCCR2A = 0x02;//set to CTC mode
	TCCR2B = 0x07;//prescaler 1024
	while ((TIFR2&(1<<OCF2A))==0);
	TCCR2A = 0;
	TCCR2B = 0;
	TIFR2 = 0x02;//clear

}

void T2D02s(){ //8000us, 25times = 0.2s
	for (int i=0; i <= 24; i++){
		T2Delay();
	}
}

void T2D1s(){ //8000us, 125times = 1s
	for (int i=0; i <= 124; i++){
		T2Delay();
	}
}

ISR (TIMER2_COMPA_vect){
	//T2MatchInterruptDisable();
	//INT1MatchInterruptEnable();
}
//INT0-----------------------------------------------------------------------------
int pressCount =0;
int state =0;

void INT0set(char inp){
	switch(tolower(inp)){//case insensitive
		case 'r':{//rising edge
			EICRA |= (1 << ISC01);
			EICRA |= (1 << ISC00);
		break;}
		case 'f':{//falling edge
			EICRA |= (1 << ISC01);
			EICRA &= ~(1 << ISC00);
		break;}
		case 'a':{//any change
			EICRA &= ~(1 << ISC01);
			EICRA |= (1 << ISC00);
		break;}
		case 'l':{//any change
			EICRA &= ~(1 << ISC01);
			EICRA &= ~(1 << ISC00);
		break;}
	}
}

//IN0 Interrupt Enable
void INT0MatchInterruptEnable(){
	EIMSK |= (1 << INT0);
	//sei();//in case you forgot la :p
}

//IN0 Interrupt Disable
void INT0MatchInterruptDisable(){
	EIMSK &= ~(1 << INT0);
}

ISR(INT0_vect){
	INT0MatchInterruptDisable();
	LEDa1Toggle();
	INT0MatchInterruptEnable();
	//T2DelayInt();
	//T2MatchInterruptEnable();
}
//END of INT0 stuff-----------------------------------------------------------------------------

int main(void)
{
	LEDsetup();
    /* Replace with your application code */
	INT0set('F');
	INT0MatchInterruptEnable();
	sei();
    while (1) 
    {
		//T2D1s();
		//LEDa1Toggle();
		//LEDa2Toggle();
		//LEDb1Toggle();
		//LEDb2Toggle();
		
    }
}

