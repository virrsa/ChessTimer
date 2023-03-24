#pragma once
#include <Arduino.h>

// Defines Variables
#define MAX_TEXT 200 //max string size
#define BAUD_RATE 0x0067     //9600 Baud rate

//Function Prototypes
void USART_init(void);
void USART_send(unsigned char data);
void USART_send_string(char* str);
unsigned char USART_receive(void);
void USART_get_string(char* str);