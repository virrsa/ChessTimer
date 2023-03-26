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
volatile bool change = false; // Keep track of whether there was a change, to perform actions in the while loop as needed
volatile bool currPlayer = false; // True if P1's turn, false if P2's - Initial button press flips from P2 to start with P1

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
    //For testing the different components without interrupts

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

    if(change) { // If something is different for this loop, perform actions as needed
      change = false; // Reset the flag because the change is being addresses
      change_led(currPlayer); // Make sure the LED is lit up for the correct player
    }
  }
}

// Button input interrupt to swap player turns and timer countdowns
ISR(INT0_vect) {
  change = true; // Something changed, so let the main game loop know
  if(currPlayer) { // If player 1 ended their turn, it's now player 2's turn
    currPlayer = false;
  }
  else { // If player 2 ended their turn, it's now player 1's turn
    currPlayer = true;
  }
}