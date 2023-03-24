#include <Peripherals.h>

// Set up the ports for the LEDs and the buzzer
void peripherals_init(void) { 
  DDRC |= (1 << GREEN_LED) | (1 << BLUE_LED); // Set LED pins to output
  DDRD |= (1 << BUZZER); // Set buzzer pin for output
  PORTC &= ~((1 << GREEN_LED) | (1 << BLUE_LED)); // Make sure output for LEDs is LOW at the start
  PORTD &= ~(1 << BUZZER); // Make sure the buzzer is off at the start
}