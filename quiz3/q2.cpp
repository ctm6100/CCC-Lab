#include <avr/io.h>
#include "avr/interrupt.h"
#include <string.h>


//Timer0-----------------------------------------------------------------------------
//global Var for timer0
bool timer0Ready = false;
int timer0Loop=0;

//Timer0 Output Compare Match A Interrupt Enable
void T0MatchInterruptEnable(){
	TIMSK0 |= (1 << OCIE0A);
	//sei();//use this OR set optimization level to NONE OR BOTH
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


const long Fosc = 16000000;//clock speed

void configSerialSpeed(long inp){
	switch(inp){
		case 2400:
		if ((UCSR0A >> U2X0)& 0x01){
			UBRR0 = 832;
			}else{
			UBRR0 = 416;
		}
		break;
		case 4800:
		if ((UCSR0A >> U2X0)& 0x01){
			UBRR0 = 416;
			}else{
			UBRR0 = 207;
		}
		break;
		
		case 9600:
		if ((UCSR0A >> U2X0)& 0x01){
			UBRR0 = 207;
			}else{
			UBRR0 = 103;
		}
		break;
		
		case 14400:
		if ((UCSR0A >> U2X0)& 0x01){
			UBRR0 = 138;
			}else{
			UBRR0 = 68;
		}
		break;
		
		case 19200:
		if ((UCSR0A >> U2X0)& 0x01){
			UBRR0 = 103;
			}else{
			UBRR0 = 51;
		}
		break;
		
		case 115200:
		if ((UCSR0A >> U2X0)& 0x01){
			UBRR0 = 16;
			}else{
			UBRR0 = 8;
		}
		break;
	}
}

void configCharSize(int inp){
	switch(inp){
		case 5://000
		UCSR0B &= ~(1 << UCSZ02);
		UCSR0C &= ~(1 << UCSZ01);
		UCSR0C &= ~(1 << UCSZ00);
		break;
		case 6://001
		UCSR0B &= ~(1 << UCSZ02);
		UCSR0C &= ~(1 << UCSZ01);
		UCSR0C |= (1 << UCSZ00);
		break;
		case 7://010
		UCSR0B &= ~(1 << UCSZ02);
		UCSR0C |= (1 << UCSZ01);
		UCSR0C &= ~(1 << UCSZ00);
		break;
		case 8://011
		UCSR0B &= ~(1 << UCSZ02);
		UCSR0C |= (1 << UCSZ01);
		UCSR0C |= (1 << UCSZ00);
		break;
		case 9://111
		UCSR0B |= (1 << UCSZ02);
		UCSR0C |= (1 << UCSZ01);
		UCSR0C |= (1 << UCSZ00);
		break;
	}
}

void RXEnable(){
	UCSR0B |= (1 << RXEN0);
}

void RXDisable(){
	UCSR0B &= ~(1 << RXEN0);
}

void TXEnable(){
	UCSR0B |= (1 << TXEN0);
}

void TXDisable(){
	UCSR0B &= ~(1 << TXEN0);
}

void RXCompInterruptEnable(){
	UCSR0B |= (1 << RXCIE0);
}

void RXCompInterruptDisable(){
	UCSR0B &= ~(1 << RXCIE0);
}

//use UDRE-------------------
void TXCompInterruptEnable(){
	UCSR0B |= (1 << TXCIE0);
}
//use UDRE-------------------
void TXCompInterruptDisable(){
	UCSR0B &= ~(1 << TXCIE0);
}
//use UDRE-------------------

void UDRECompInterruptEnable(){
	UCSR0B |= (1 << UDRIE0);
}

void UDRECompInterruptDisable(){
	UCSR0B &= ~(1 << UDRIE0);
}

void UArtSendPolling(unsigned char inp){
	while ( !(UCSR0A & (1<<UDRE0)) ){ //0000 & 0010 = 0000(->!false) , 0010 and 0010 = 0010(->!true
	}//Wait for UDR empty i.e. ready to send
	UDR0 = inp;
}

void UArtSendPollingString(char *inp){
	int len = strlen(inp);
	for (int i=0; i < len; i++){
		UArtSendPolling(inp[i]);
	}
}

char UArtReceivePolling(void){
	while ( !(UCSR0A & (1<<RXC0)) ){ //0000 & 0010 = 0000(->!false) , 0010 and 0010 = 0010(->!true
	}//Wait for data to be received
	return UDR0;
}


//default string does no matter
char *UartSend = "We Are Ready!";
//TX_i store the number of Char still need to send
int TX_i= 0;
//TXonce determine weather it send once or keep sending,
//i.e. TXonce is true send once time the string(char arry pointer) then disable the send interrupt(UDRE)
bool TXonce= false;

void UArtSendInterrupt(char *inp){
	TX_i= strlen(inp);
	UartSend = inp;
	TXonce = false;
	UDRECompInterruptEnable();
}

void UArtSendInterruptOnce(char *inp){
	TX_i= strlen(inp);
	UartSend = inp;
	TXonce= true;
	UDRECompInterruptEnable();
}

//bool isUartSend = false;
//ISR USART_UDRE_vect is petty well define, use UArtSendInterrupt() and UArtSendInterruptOnce() la
ISR(USART_UDRE_vect){
	if (TX_i >=0){
		//send the current char
		UDR0 = UartSend[strlen(UartSend) - (TX_i--)];
		}else{
		if (TXonce){
			UDRECompInterruptDisable();
		}
		TX_i=strlen(UartSend);
	}
}

//do not use this one
/*
ISR(USART_TXC_vect){
}
*/
char *sentChar ="loop!!!!";

bool isUartReceive= false;
char UartReceive;
char UartLastReceive;//lab4 only stuff
int RX_stat=0;//lab4 only stuff
char *temp = "A";//lab4 only stuff
ISR(USART_RX_vect){
	//copy the input char to UartReceive
	UartReceive = UDR0;
	
	//do the stuff you want to do here---------------
	//p.s. keep it short
	if (UartReceive == 'X'){
		T0MatchInterruptDisable();
	}else if (UartReceive == 'Y'){
		T0MatchInterruptEnable();
	}
}

int main(void)
{
	//timer-----------------
	T0DelayInterrputSetup();
	T0MatchInterruptEnable();
	
	//Uart 
	configSerialSpeed(9600);
	configCharSize(8);
	RXEnable();//RX pin enable
	TXEnable();//TX pin enable
	
	//code start here
	RXCompInterruptEnable();
	sei();
	
	while (1)
	{
		
	}
}

//content for Timer0 Output Compare Match A Interrupt(ISR: TIMER0_COMPA_vect)
ISR (TIMER0_COMPA_vect){
	if (timer0Loop > 0){// if > 0 than loop
		timer0Loop--;
	}else{
		D1s();
		timer0Ready= true;
		UArtSendInterruptOnce(temp);
		//T0MatchInterruptDisable();// disable the on99 interrupt:(
	}
}
