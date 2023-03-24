#include "ShiftReg.h"

void init_shift(byte dataPin, byte clockPin, byte latchPin)
{
  DDRD |= (1<<dataPin) | (1<<latchPin) | (1<<clockPin); // Port D PIN4,5, and 6 set to ouput 
}


//function to generate a byte from a serial data in shift register
void myShiftOut(byte dataPin, byte clockPin, byte bitOrder, byte val)
{
  byte i;
  for (i = 0; i < 8; i++)  
  {
      if (bitOrder == LSBFIRST) {
        if((val & 1)) //if the bit is 1
        {
          PORTD|= (1<<dataPin); //send one
        }
        else
        {
          PORTD&= ~(1<<dataPin); //send 0
        }   
        val >>= 1; //shift the data to right to get the next LSB
      } else {
        if((val & 128) != 0)
        {
          PORTD|= (1<<dataPin);
        }
        else
        {
          PORTD&= ~(1<<dataPin);
        }   
        val <<= 1; //shift the data to left to get the next MSB
      }
      //Create a clock pulse
      PORTD |= (1<<clockPin);
      PORTD &= ~(1<<clockPin);
  }
}
