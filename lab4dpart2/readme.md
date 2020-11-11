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
noramlly 8 but up to you
```C++
void configCharSize(int inp)
```
---
