#include <Arduino.h>
#include <avr_debugger.h>
#include <USART.h>
#include <LCD.h>
#include <INT0.h>
#include <Segment.h>
#include <ShiftReg.h>
#include <Peripherals.h>

char text[MAX_TEXT]; // for USART input
// Output values to display numbers on 7-seg
byte digits[ARRAY_SIZE_DECIMAL] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6, // 0 - 9
                                   0xEE, 0x3E, 0x9C, 0x7A, 0x9E, 0x8E}; // A - F

int main() {

  #ifdef __DEBUG__
    dbg_start();
  #endif

  // initialize USART/LCD/INT0/shift register controller
  USART_init();
  LCD_init();
  INT0_init();
  peripherals_init();
  init_shift(DATA,CLOCK,LATCH);

  // clear LCD display
	LCD_command(1);

  // reset variables
	memset(text, 0, MAX_TEXT);

  LCD_string("Test");

  while(1) {
    /* 
    //For testing the different components

    change_led(true);
    _delay_ms(500);
    change_led(false);
    _delay_ms(500);
    for(int i = 0; i < 16; i++) {
      displayValue(digits[i]);
      _delay_ms(500);
    }
    toggle_buzzer(true);
    _delay_ms(100);
    toggle_buzzer(false);
    */
  }
}

// Button input interrupt to swap player turns and timer countdowns
ISR(INT0_vect) {

}