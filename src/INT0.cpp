#include <INT0.h>

void INT0_init(void) {
  DDRD &= ~(1 << BUTTON); // set button to input
  PORTD |= (1 << BUTTON); // enable internal pull up resistor 

  EICRA|= (1 << ISC01); // set INT0 for falling edge mode
  EIMSK|=(1 << INT0); // enable INT0
  sei(); // enable global interrupt in SERG
}