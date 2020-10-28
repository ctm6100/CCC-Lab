/*
 * q2.cpp
 *
 * Created: 10/28/2020 9:16:45 AM
 * Author : CHONG Tsz Man 18033255d
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
	
	LEDa1on();
	//LEDa2on();
	
	//LEDb1on();
	//LEDb2on();
	
}

//Timer0-----------------------------------------------------------------------------
//global Var for timer0
double T=2;
bool timer0Ready = false;
int timer0Loop=0;

//Timer0 Output Compare Match A Interrupt Enable
void T0MatchInterruptEnable(){
	TIMSK0 |= (1 << OCIE0A);
	//use this OR set optimization level to NONE OR BOTH
}

//Timer0 Output Compare Match A Interrupt Disable
void T0MatchInterruptDisable(){
	TIMSK0 &= ~(1 << OCIE0A);
}

void timer0SetCTC(){
	//TCCR0A = 0x02; //A:**** **10 B:**** 0***, 0 10 CTC mode
	TCCR0B &= ~(1 << WGM02);
	TCCR0A |= (1 << WGM01);
	TCCR0A &= ~(1 << WGM00);
}

void timer0SetPrescale1024(){
	//**** *101 => Prescale1024,
	TCCR0B |= (1 << CS02);
	TCCR0B &= ~(1 << CS01);
	TCCR0B |= (1 << CS00);
}

void timer0SetRising(){
	//**** *11! => Rising edge,
	TCCR0B |= (1 << CS02);
	TCCR0B |= (1 << CS01);
	TCCR0B |= (1 << CS00);
}

void timer0SetFalling(){
	//**** *110 => falling edge,
	TCCR0B |= (1 << CS02);
	TCCR0B |= (1 << CS01);
	TCCR0B &= ~(1 << CS00);
}

void T0DelayInterrputSetup(){ //each CTC match = 8000us, for interupt
	OCR0A = 124;//125-1
	timer0SetCTC();
	timer0SetPrescale1024();
}

void D05s(){ //8000us, loop 25times = 0.2s
	timer0Loop = 92;
}

void D2s(){ //8000us, loop 125times = 1s
	timer0Loop = 125*2;
}

//content for Timer0 Output Compare Match A Interrupt(ISR: TIMER0_COMPA_vect)
ISR (TIMER0_COMPA_vect){
	if (timer0Loop > 0){// if > 0 than loop
		timer0Loop--;
	}else{
		timer0Ready= true;
		if (T == 2){
			D2s();
		}else{
			D05s();
		}
		LEDa1Toggle();
		LEDa2Toggle();
	}
}
//End of Timer0-----------------------------------------------------------------------------

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
	if ( T== 2){
		T=0.5;
	}else{
		T=2;
	}
	//LEDa1Toggle();
	//LEDa2Toggle();
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
	D2s();
	T0DelayInterrputSetup();
	
	T0MatchInterruptEnable();
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

