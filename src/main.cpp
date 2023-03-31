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
long getOverallTime(); // If running mode 1, ask the user to input the total time players will get
long getTurnTime(); // If running mode 2, ask the user to input the maximum time per turn, up to 16s
bool validateTime(char* input); // Function to validate the tokenized items in getOverallTime

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

  int mode = selectMode(); // Get user input to select the mode the timer will run in
  long seconds; // Keep track of the total number of seconds for timing, whether it be overall or per turn
  if(mode == 1) // If playing mode 1, get the overall turn time for the players
    seconds = getOverallTime();
  else // If playing mode 2, get the maximum turn time for each player
    seconds = getTurnTime();
  
  // Just for debugging
  memset(text, 0, MAX_TEXT);
  USART_send_string("\n=====================================================\n");
  sprintf(text, "Time is %ld", seconds);
  USART_send_string(text);
  memset(text, 0, MAX_TEXT);

  gameStarted = true; // Mode and time have been specified, button INT0 interrupt can start working

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

// Prompt the user to select the game mode using the serial console
int selectMode() {

  char modeStr[MAX_TEXT]; // To store the mode the user selected
  
  // Print out a message to the LCD telling the user to select a mode
  LCD_string("Select the mode:");
  LCD_command(0xC0); // Move cursor to 2nd line for 2nd half of message
  LCD_string("(Serial console)");

  promptMode(); // Print the available mode information out to the user
  USART_get_string(modeStr); // Read the user input from serial
  int modeInt = atoi(modeStr); // Convert the input to an integer

  while(strlen(modeStr) > 1 || modeInt != 1 && modeInt != 2) { // Make sure the selected mode is valid
    USART_send_string("\n=====================================================\n");
    USART_send_string("Invalid unput! Please enter either '1' or '2'.\n");
    promptMode(); // Print the available mode information out to the user
    memset(modeStr, 0, MAX_TEXT); // Make sure array is clean before taking input
    USART_get_string(modeStr); // Read the user input from serial
    modeInt = atoi(modeStr); // Convert the input to an integer
  }

  return modeInt; // Return the user's selected mode
}

// Used to print out the mode options, function was made to reduce a few lines used
void promptMode() { 
  USART_send_string("Please select a mode:\n");
  USART_send_string(" - Mode 1: Limited overall time, unlimited turn time\n");
  USART_send_string(" - Mode 2: Unlimited overall time, limited turn time\n");
  USART_send_string("Enter the number for the desired mode: ");
}

// Prompt the user for the overall player time if playing mode 1
long getOverallTime() {
  bool validInput = false; // Used to loop until input is valid
  long totalSeconds; // To store the return value containing the converted total number of seconds 
  char* element; // To keep track of the token currently being parsed

  // Doing the input validation and the math in one location for the sake of efficiency
  // The string has to be tokenized for both validation and conversion to seconds, so we tokenize once instead of twice
  while(!validInput) {
    memset(text, 0, MAX_TEXT); // Make sure input string is empty before using it to grab input
    USART_send_string("\n=====================================================\n");
    USART_send_string("Please input the overall time each user will get.\n");
    USART_send_string("Format as 'hh:mm:ss': ");
    USART_get_string(text); // Retrieve the time input string from the user via serial

    totalSeconds = 0; // Make sure to clear totalSeconds from any previous iteration's values if validation failed
    element = strtok(text, ":"); // Input is delimited by colons, so use that for tokenizing

    if(validateTime(element)) // If hh input is valid, add it to total number of seconds
      totalSeconds += 3600L * atoi(element);
    else
      continue;
    element = strtok(NULL, ":"); // Move to next token
    if(validateTime(element)) // If mm input is valid, add it to total number of seconds
      totalSeconds += 60L * atoi(element);
    else
      continue;
    element = strtok(NULL, ":"); // Move to next token
    if(validateTime(element)) // If ss input is valid, add it to total number of seconds
      totalSeconds += atoi(element);
    else
      continue;
    element = strtok(NULL, ":"); // See if there is another token to check if more than 3 input values were provided
    if(element != NULL) // If more than 3 input values were provided, fail validation
      continue;
    validInput = true; // If all validation steps passed, return the calculated seconds value
  }
  return totalSeconds; // If all validations passed, return the total number of seconds
}

// Function to validate the tokenized items in getOverallTime - Done multiple times, so made a function to reduce needed lines
bool validateTime(char* input) {
  if(strlen(input) != 2) // Return false if anythign other than 2 characters/numbers were provided
    return false;
  for(int i = 0; i < strlen(input); i++) // Make sure every number input is a digit, otherwise fail validation
    if(!isdigit(input[i]))
      return false;
  return true; // Input is valid if everything else passed
}

// Prompt the user for maximum turn time if playing mode 2
long getTurnTime() {
  int turnTime; // Stores the return value for the maximum allowed time per turn

  while(!(turnTime >= 1 && turnTime <= 16)) { // Prompt the user for a max turn time until the input is between 1 and 16
    memset(text, 0, MAX_TEXT); // Make sure input string is empty before using it to grab input
    USART_send_string("\n=====================================================\n");
    USART_send_string("Please enter the maximum allowed time per player turn.\n");
    USART_send_string("Seconds (1 - 16): ");
    USART_get_string(text); // Read the user's input from serial
    turnTime = atoi(text); // Return value is 0 if string is not a number, so the while loop will rerun
  }
  return turnTime; // Return the user-provided turn time is input is valid
}

// Button input interrupt to swap player turns and timer countdowns
ISR(INT0_vect) {
  if(gameStarted) { // Prevent interrupts from doing anything until a mode and time have been specified
    change = true; // Something changed, so let the main game loop know
    if(currPlayer) // If player 1 ended their turn, it's now player 2's turn
      currPlayer = false;
    else // If player 2 ended their turn, it's now player 1's turn
      currPlayer = true;
  }
}