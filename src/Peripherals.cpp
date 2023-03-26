#include <Peripherals.h>

// Set up the ports for the LEDs and the buzzer
void peripherals_init(void) { 
  DDRC |= (1 << GREEN_LED) | (1 << BLUE_LED); // Set LED pins to output
  DDRD |= (1 << BUZZER); // Set buzzer pin for output
  PORTC &= ~((1 << GREEN_LED) | (1 << BLUE_LED)); // Make sure output for LEDs is LOW at the start
  PORTD &= ~(1 << BUZZER); // Make sure the buzzer is off at the start
}

void change_led(bool player) {
  if(player) { // If it's player 1's turn, turn their LED on and turn player 2's LED off
    PORTC |= (1 << GREEN_LED);
    PORTC &= ~(1 << BLUE_LED);
  }
  else { // Player 2's turn, turn player 1's LED off and player 2's LED on
    PORTC |= (1 << BLUE_LED);
    PORTC &= ~(1 << GREEN_LED);
  }
}

void toggle_buzzer(bool buzz) {
  if(buzz) { // If buzzer on, set to HIGH
    PORTD |= (1 << BUZZER);
  }
  else { // Otherwise set to LOW
    PORTD &= ~(1 << BUZZER);
  }
}