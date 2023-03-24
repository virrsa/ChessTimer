#pragma once
#include <Arduino.h>

// Defines Variables
#define RS DDB1 // Register Select is connected to PB1
#define EN DDB0 // Enable pin is connected to PB0

#define D4 DDC0 // D4 is connected to PC0
#define D5 DDC1 // D5 is connected to PC1
#define D6 DDC2 // D6 is connected to PC2
#define D7 DDC3 // D7 is connected to PC3


// Functions Prototypes
void delayMs(uint16_t ms);

// initialize the library with the numbers of the interface pins
void LCD_nibble_write(uint8_t data, uint8_t control);
void LCD_command(uint8_t command);
void LCD_data(uint8_t data);
void LCD_init(void);
void PORTS_init(void);
void LCD_string(char *str);