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
bool gameStarted = false; // Only perform button interrupt actions if the game has started

int selectMode(); // Takes USART input to allow the user to configure the timer
void promptMode(); // Prints out the mode options to serial to prompt the user to select one

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

	LCD_command(1); // Make sure the LCD is clear to start off

  displayValue(0); // Make sure 7-seg is off to start

	memset(text, 0, MAX_TEXT); // Make sure variables are clear (they should be, but just to be safe)

  selectMode(); // Get user input to select the mode the timer will run in

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

int selectMode() {
  
  char modeStr[MAX_TEXT]; // To store the mode the user selected
  
  // Print out a message to the LCD telling the user to select a mode
  LCD_string("Select the mode:");
  LCD_command(0xC0); // Move cursor to 2nd line for 2nd half of message
  LCD_string("(Serial console)");

  promptMode(); // Print the available mode information out to the user
  USART_get_string(modeStr); // Read the user input from serial
  int modeInt = atoi(modeStr); // Convert the input to an integer

  while(modeInt != 1 && modeInt != 2 || strlen(modeStr) > 1) { // Make sure the selected mode is valid
    USART_send_string("\n=====================================================\n");
    USART_send_string("Invalid unput! Please enter either '1' or '2'.\n");
    promptMode(); // Print the available mode information out to the user
    memset(modeStr, 0, MAX_TEXT); // Make sure array is clean before taking input
    USART_get_string(modeStr); // Read the user input from serial
    modeInt = atoi(modeStr); // Convert the input to an integer
  }

  sei(); // Only take button input once the mode has been selected, so that the game can start
}

void promptMode() { // Used to print out the mode options, function was made to reduce a few lines used
  USART_send_string("Please select a mode:\n");
  USART_send_string(" - Mode 1: Limited overall time, unlimited turn time\n");
  USART_send_string(" - Mode 2: Unlimited overall time, limited turn time\n");
  USART_send_string("Enter the number for the desired mode: ");
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