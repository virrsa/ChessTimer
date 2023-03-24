#pragma once
#include <Arduino.h>

#define LATCH PD5  // pins 5 of Arduino for Latch pin of shift register
#define CLOCK PD6  // pins 6 of Arduino for clock pin of shift register 
#define DATA PD4   // pins 4 of Arduino for data pin of shift register

// Function Prototypes
void init_shift(byte dataPin, byte clockPin, byte latchPin);

//function to generate a byte from a serial data in shift register
void myShiftOut(byte dataPin, byte clockPin, byte bitOrder, byte val);
