
#include <avr/io.h>
#include "avr/interrupt.h"
#include <ctype.h>

//LED GPIO---------------------------------------------------------------------------
//LEDa1 -> PB0 (D8)
//LEDa2 -> PB1 (D9)
//LEDb1 -> PC4 (A4)
//LEDb1 -> PC5 (A5)

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
	
	LEDb1on();
	//LEDb2on();
}
//LED GPIO---------------------------------------------------------------------------


//Timer0-----------------------------------------------------------------------------
//global Var for timer0
bool timer0Ready = false;
int timer0Loop=0;

//Timer0 Output Compare Match A Interrupt Enable
void T0MatchInterruptEnable(){
	TIMSK0 |= (1 << OCIE0A);
	sei();//use this OR set optimization level to NONE OR BOTH
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

void D02s(){ //8000us, loop 25times = 0.2s
	timer0Loop = 25;
}

void D1s(){ //8000us, loop 125times = 1s
	timer0Loop = 125;
}

//content for Timer0 Output Compare Match A Interrupt(ISR: TIMER0_COMPA_vect)
ISR (TIMER0_COMPA_vect){
	static int timer0State =0;
	
	if (timer0Loop > 0){// if > 0 than loop
		timer0Loop--;
	}else{
		D1s();
		//timer0Ready= true;
		if (timer0State == 0){//state X
			LEDb1on();
			LEDb2on();
			LEDa1off();
			timer0State++;
		}else if (timer0State == 1){
			LEDb1off();
			LEDb2off();
			LEDa1on();
			timer0State++;
		}else{
			LEDb1on();
			LEDb2on();
			LEDa1on();
			timer0State=0;
		}
		//T0MatchInterruptDisable();// disable the on99 interrupt:(
	}
}
//End of Timer0-----------------------------------------------------------------------------

int LEDState =0;
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
	//ISR content
	if (LEDState == 0){
		LEDState =1;
		T0MatchInterruptDisable();
		LEDb1off();
		LEDb2off();
		LEDa1off();
	}else{
		LEDState =0;
		T0MatchInterruptEnable();
	}
	INT0MatchInterruptEnable();
	//T2DelayInt();
	//T2MatchInterruptEnable();
}
//END of INT0 stuff-----------------------------------------------------------------------------
int main(void)
{
	LEDsetup();
	/* Replace with your application code */
	
	D1s();
	T0DelayInterrputSetup();
	
	T0MatchInterruptEnable();
	
	INT0set('f');
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


