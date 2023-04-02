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
volatile bool currPlayer = true; // True if P1's turn, false if P2's

// Keep track of the amount of turns the players have had
int p1_turns = 1; // Player 1 starts the game at turn 1, turns incremented on switch
int p2_turns = 0; // Player 2 starts at 0 turns, which increments on switch

// Used for converting seconds to hh:mm:ss
int hrs = 0;
int mins = 0;
int secs = 0;

int selectMode(); // Takes USART input to allow the user to configure the timer
void promptMode(); // Prints out the mode options to serial to prompt the user to select one
long getOverallTime(); // If running mode 1, ask the user to input the total time players will get
long getTurnTime(); // If running mode 2, ask the user to input the maximum time per turn, up to 16s
bool validateTime(char* input); // Function to validate the tokenized items in getOverallTime
void mode_1(long seconds); // Activates mode 1 chess timer (unlimited turn time with limited time overall)
void mode_2(long seconds); // Activates mode 2 chess timer (limited turn time with unlimited time overall)
void displayWinner(bool player); // When the game ends, stop the loop and display the winner to the LCD

int main() {

  #ifdef __DEBUG__
    dbg_start();
  #endif

  // initialize USART/LCD/INT0/shift register controller
  USART_init();
  LCD_init();
  peripherals_init();
  init_shift(DATA,CLOCK,LATCH);
  TCCR1B = (1 << CS12); // set prescalar for timer1 (256)

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

  INT0_init(); // Mode and time have been specified, button INT0 interrupt can start working

  change_led(currPlayer); // Turn on inital LED
  TIFR1 |= (1 << OCF1A); // Reset timer1 overflow flag

  // Loop for playing mode 1
  if (mode == 1) {
    // Activate mode 1 chess timer
    mode_1(seconds);
  }
  // Loop for playing mode 2
  else if (mode == 2) {
    // Activate mode 2 chess timer
    mode_2(seconds);
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
    USART_send_string("Invalid input! Please enter either '1' or '2'.\n");
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

void mode_1(long seconds) {

  // For the sake of using less variables, p1 time is tracked using "seconds"
  long p2_seconds = seconds; // P2 has same starting time as P1, and their own remaining seconds counter

  while(1) {
    if(change) { // If something is different for this loop, perform actions as needed
      change = false; // Reset the flag because the change is being addressed
      change_led(currPlayer); // Make sure the LED is lit up for the correct player
      continue; // Start next loop so that the player has their timer start from fresh
    }

    // Display remaining time for players

    LCD_command(0x80);  // move to first line
    LCD_command(0x01); // Clear LCD display
    if(currPlayer) {
      // Convert seconds to hh:mm:ss
      hrs = seconds / 3600;
      mins = (seconds % 3600) / 60;
      secs = (seconds % 3600) % 60;

      LCD_string("Player 1:");
    }
    else {
      // Convert seconds to hh:mm:ss
      hrs = p2_seconds / 3600;
      mins = (p2_seconds % 3600) / 60;
      secs = (p2_seconds % 3600) % 60;

      LCD_string("Player 2:");
    }
    sprintf(text, "%i:%i:%i", hrs, mins, secs); // Format time for LCD
    LCD_command(0xC0); // Move to second line
    LCD_string(text); // Display time remaining for player
    memset(text, 0, MAX_TEXT); // Clear string 

    TCNT1 = 3035; // Initialize timer value for 1000ms
    // TODO - Maybe do a change check and break within this while loop as well?
    // Might make for faster responses, things seem a bit delayed
    while((TIFR1 & (1 << OCF1A)) == 0); // Check if overflow flag is set
    TIFR1 |= (1 << OCF1A) ; // Reset timer1 overflow flag

    if(currPlayer) // If player 1 had 1s elapsed, decrease remaining time
      seconds --;
    else // If P2 had 1s elapsed, decrease remaining time
      p2_seconds--;

    if(seconds == 0 || p2_seconds == 0){ // If one of the players ran out of time, game is over
      cli(); // Disable interrupts to prevent button of messing with end of game sequence
      displayWinner(!currPlayer); // Winner is opposite of currPlayer, as currPlayer ran out of time
      break; // Game is done
    }
  }
}

void mode_2(long seconds) {

  while(1) {

    // Display player turn information
    LCD_command(0x80);  // move to first line
    LCD_command(0x01); // Clear LCD display
    sprintf(text, "P1: %i Turns", p1_turns);
    LCD_string(text);
    memset(text, 0, MAX_TEXT); // Clear string
    LCD_command(0xC0); // Move to second line
    sprintf(text, "P2: %i Turns", p2_turns);
    LCD_string(text);
    memset(text, 0, MAX_TEXT); // Clear string

    // Count down the time left for the player
    for (int i = seconds-1; i >= 0; i--) {
      if (change == true) { // If button press is detected, break out of the loop
        break;
      }
      displayValue(digits[i]); // Display time left on 7-segment display
      TCNT1 = 3035; // Initialize timer value for 1000ms
      while((TIFR1 & (1 << OCF1A)) == 0); // Check if overflow flag is set
      TIFR1 |= (1 << OCF1A) ; // Reset timer1 overflow flag
    }

    if(change) { // If something is different for this loop, perform actions as needed
      change = false; // Reset the flag because the change is being addresses
      if (currPlayer) { p1_turns++; } // If currently player 1, increase turns
      else { p2_turns++; } // If currently player 2, increase turns
      change_led(currPlayer); // Make sure the LED is lit up for the correct player
    }
    else { // If the player runs out of time, they lose
      displayWinner(!currPlayer);
      break; // Game is done
    }
  }
}

void displayWinner(bool player) {

  // Game is done, run the buzzer for 1s 
  toggle_buzzer(true); // Turn on buzzer
  TCNT1 = 3035; // Initialize timer1 value for buzzer to buzz for one second
  while((TIFR1 & (1 << OCF1A)) == 0); // Check if overflow flag is set
  TIFR1 |= (1 << OCF1A); // Reset timer1 overflow flag
  toggle_buzzer(false); // Turn off buzzer
  
  // Game is over, turn off both LEDs
  PORTB &= ~(1 << GREEN_LED);
  PORTB &= ~(1 << BLUE_LED);
  PORTD &= ~(1 << BUZZER); // Make sure the buzzer is off
  displayValue(0); // Make sure 7-seg is off
  
  // Display winner on LCD
  LCD_command(0x80);  // move to first line
  LCD_command(1); // Clear LCD
  LCD_string("Game over");
  LCD_command(0xC0); // Move to second line
  if (player) { LCD_string("Player 1 Wins"); }
  else { LCD_string("Player 2 wins"); }
}

// Button input interrupt to swap player turns and timer countdowns
ISR(INT0_vect) {
  change = true; // Something changed, so let the main game loop know
  if(currPlayer) // If player 1 ended their turn, it's now player 2's turn
    currPlayer = false;
  else // If player 2 ended their turn, it's now player 1's turn
    currPlayer = true;
}