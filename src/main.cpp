#include <Arduino.h>
#include <avr_debugger.h>
#include <USART.h>
#include <LCD.h>
#include <INT0.h>
#include <Segment.h>
#include <ShiftReg.h>
#include <Peripherals.h>

char text[MAX_TEXT]; // for USART input
byte digits[ARRAY_SIZE_DECIMAL] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6 }; // for 7-segment digits

int main() {

  #ifdef __DEBUG__
    dbg_start();
  #endif

  // initialize USART/LCD/INT0 controller
  USART_init();
  LCD_init();
  INT0_init();
  peripherals_init();

  // clear LCD display
	LCD_command(1);

  // reset variables
	memset(text, 0, MAX_TEXT);

  while(1) {

  }
}

// Button input interrupt to swap player turns and timer countdowns
ISR(INT0_vect) {

}