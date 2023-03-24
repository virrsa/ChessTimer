#include "Segment.h"
#include "ShiftReg.h"


void displayValue(byte value)
{
  //set latch to low
  PORTD&=~(1<<LATCH);
  //shift the data in shift register
  myShiftOut(DATA, CLOCK, LSBFIRST, value);
  //set the latch to high again
  PORTD|=(1<<LATCH);
}
