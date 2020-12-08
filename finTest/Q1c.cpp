
#include <avr/io.h>
#include "avr/interrupt.h"
#include <string.h>

const int Fosc = 16000000;//clock speed

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
	
	LEDa1off();
	LEDa2off();
	
	LEDb1off();
	LEDb2off();
}
//LED GPIO---------------------------------------------------------------------------

void configSerialSpeed(int inp){
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

void TXCompInterruptEnable(){
	UCSR0B |= (1 << TXCIE0);
}

void TXCompInterruptDisable(){
	UCSR0B &= ~(1 << TXCIE0);
}

void UDRECompInterruptEnable(){
	UCSR0B |= (1 << UDRIE0);
}

void UDRECompInterruptDisable(){
	UCSR0B &= ~(1 << UDRIE0);
}

void UArtSendPolling(unsigned char inp){
	while ( !(UCSR0A & (1<<UDRE0)) ){ //0000 & 0010 = 0000(->!false) , 0010 and 0010 = 0010(->!true
	}//Wait for data to be received
	UDR0 = inp;
}

char UArtReceivePolling(void){
	while ( !(UCSR0A & (1<<RXC0)) ){ //0000 & 0010 = 0000(->!false) , 0010 and 0010 = 0010(->!true
	}//Wait for data to be received
	return UDR0;
}



char *UartSend = "We Are Ready!";
int TX_i= 0;

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
ISR(USART_UDRE_vect){
	if (TX_i !=0){
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
char *sentChar ="a";

bool isUartReceive= false;
char UartReceive;
char UartLastReceive;
int RX_stat=0;
char *temp = "";
int LEDonState =0, LEDoffState=0;
ISR(USART_RX_vect){
	UartReceive = UDR0;

	if (LEDonState == 0){
		if (UartReceive == 'x'){
			LEDonState =1;
		}else{
			LEDonState =0;
		}
	} else if (LEDonState == 1){
		if (UartReceive == 'y'){
			LEDonState =2;
		}else{
			LEDonState =0;
		}
	} else if (LEDonState == 2){
		if (UartReceive == 'z'){
			LEDonState =0;
			LEDb1on();
			LEDb2on();
			LEDa1on();
			//UArtSendInterruptOnce("XYZ");
		}else{
			LEDonState =0;
		}
	}
	
	if (LEDoffState == 0){
		if (UartReceive == 'P'){
			LEDoffState =1;
			}else{
			LEDoffState =0;
		}
	} else if (LEDoffState == 1){
		LEDoffState =0;
		if (UartReceive == 'Q'){
			LEDb1off();
			LEDb2off();
			LEDa1off();
			//UArtSendInterruptOnce("PQ");
		}
	}
	/*
	if (RX_stat == 0){
		UDRECompInterruptDisable();
		temp = "1234567890";
		for (int i=0; i<10; i++){
			temp[i]= UartReceive;
		}
		RX_stat = 1;
		UartLastReceive =UartReceive;
		UArtSendInterruptOnce(temp);
		}else {
		if (UartLastReceive == UartReceive){
			RX_stat = 0;
			UArtSendInterrupt(sentChar);
		}
	}
	*/
}

int main(void)
{
	LEDsetup();
	
	configSerialSpeed(9600);
	configCharSize(8);
	RXEnable();//RX pin enable
	TXEnable();//TX pin enable
	RXCompInterruptEnable();
	sei();
    /* Replace with your application code */
    while (1) 
    {
    }
}

