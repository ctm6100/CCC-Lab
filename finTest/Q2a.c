#include "stm32f10x.h" // Device header
//#include "main.h"
#include "string.h"

//onboard LED
void onBoardLEDSetup(void);
void onBoardLED(void);
void offBoardLED(void);
void EXTI15_10_IRQHandler(void);

//LED

//timer 
void timerSetup(void);
void TIM2_IRQHandler(void);

//button Ex interrupt
#define BUTTON_RCC_GPIO RCC_APB2Periph_GPIOC
#define BUTTON_GPIO GPIOC
#define BUTTON_GPIO_PIN GPIO_Pin_13
#define BUTTON_EXTI_LINE EXTI_Line13
#define BUTTON_GPIO_PORTSOURCE GPIO_PortSourceGPIOC
#define BUTTON_GPIO_PINSOURCE GPIO_PinSource13
void buttonSetup(void);

//Usart
void USART2_gpioSetup(void);
void USART2_uartSetup(void);
void USART2_interruptSetup(void);
void USART2_IRQHandler(void);


char msg[] = "a";
//char bye[] = "Bye!";
static uint8_t msg_i=0;
unsigned char character;

int main(void) {
	msg_i=0;
	//led
	onBoardLEDSetup();
	onBoardLED();
	
	//button
	buttonSetup();
	
	//timer
	timerSetup();
	
	//Uart
	USART2_gpioSetup();
	USART2_uartSetup();
	USART2_interruptSetup();
	
	while(1) {
	}
}
//onboard LED---------------------------------------------------
void onBoardLEDSetup(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//PA5 on board LED
	GPIO_InitTypeDef GPIO_InitOnBoardLED;
	GPIO_InitOnBoardLED.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitOnBoardLED.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitOnBoardLED.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitOnBoardLED);
}

void onBoardLED(){
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);//Red*
}

void offBoardLED(){
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);//Red*
}
//timer-------------------------------------------------------

void timerSetup(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 18000 - 1; //1/(72Mhz/18000)=0.25ms
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 4000 - 1; //1s,  0.25ms*4000 = 1s
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);
	
	//Enable update event for Timer2
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
}

int flash =0;
int ledState =0;
void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		if(flash == 0){
			if (ledState == 0) {
				onBoardLED();
				ledState = 1; 
			}else{
				offBoardLED();
				ledState = 0; 
			}
		}
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

//EXT button--------------------------------------------------
void buttonSetup(){
	//PC13 on borad button
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	// Configure I/O for EXTI13
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = BUTTON_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BUTTON_GPIO, &GPIO_InitStruct);
	
	// EXTI Configuration, GPIOC, Pin 13
	GPIO_EXTILineConfig(BUTTON_GPIO_PORTSOURCE, BUTTON_GPIO_PINSOURCE);
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = BUTTON_EXTI_LINE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	// Enable Interrupt
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_Init(&NVIC_InitStruct);
}


void EXTI15_10_IRQHandler(void) {
	//onboardButtton
	if (EXTI_GetITStatus(EXTI_Line13) == SET) {
		if (flash == 0){
			flash = 1;
			onBoardLED();
		}else{
			flash = 0;
		}
		
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}


//Uart2---------------------------------------------------------------

void USART2_gpioSetup(){
	//USART2 TX RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void USART2_uartSetup(){
	//USART2 ST-LINK USB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
}

void USART2_interruptSetup(){
	//Uart Interrupt setup
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// Enable the USART2 TX Interrupt
	USART_ITConfig(USART2, USART_IT_TC, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	// Enable the USART2 RX Interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
static int once =0,state=0;
static char lastchar;

void USART2_IRQHandler() {
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET) {
		if(msg_i < strlen(msg)) {
		USART_SendData(USART2, msg[msg_i++]);
		}else if (once == 0){
			if (state == 1){//state ==1 stop running after send
				once =1;
			}else{
				msg_i=0;
			}
		}
		
		//USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		character = (unsigned char) USART_ReceiveData(USART2);
		//USART_SendData(USART2, character);
		if(state==0){
			state=1;
			lastchar = character;
			for (int i=0; i<10; i++){
				msg[i]= character;
			}
			msg_i=0;
		}else if (lastchar == character){
			strcpy(msg,"a");
			state =0;
			once=0;
		}
	}
}

