# Serial function
### Serial Registor Confing
#### Config TX RX Pin
##### TX(UArt0) pin enable\ disable
TX(UArt0) pin enable
```C++
void TXEnable()
```
TX(UArt0) pin disable
```C++
void RXDisable()
```

##### RX(UArt0) pin enable\ disable
RX(UArt0) pin enable
```C++
void RXEnable()
```
RX(UArt0) pin disable
```C++
void RXDisable()
```
---
### Config Serial Status
#### Serial Speed Confing
set the serial speed (e.g.9600, 115200...) will adjust for double speed
```C++
void configSerialSpeed(int inp)
```
---
#### Serial Char Size
noramlly 8 but up to you(5-9)
```C++
void configCharSize(int inp)
```
---
#### RX Interrupt
RX Interrupt Enable
```C++
void RXCompInterruptEnable(){
	UCSR0B |= (1 << RXCIE0);
}
```
RX Interrupt Disable
```C++
void RXCompInterruptDisable(){
	UCSR0B &= ~(1 << RXCIE0);
}
```
---
#### UDRE Empty Interrupt
UDRE Empty Interrupt Enable
```C++
void UDRECompInterruptEnable(){
	UCSR0B |= (1 << UDRIE0);
}
```
UDRE Empty Interrupt Disable
```C++
void UDRECompInterruptDisable(){
	UCSR0B &= ~(1 << UDRIE0);
}
```
# UArt FUNCTION
### Polling
#### UArt Send Polling
```C++
void UArtSendPolling(unsigned char inp)
```
#### UArt Receive Polling
the Receive CHAR will return in this function
```C++
char UArtReceivePolling(void)
```
---
