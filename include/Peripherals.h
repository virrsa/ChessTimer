#pragma once
#include <Arduino.h>

#define GREEN_LED PC4 // Green LED to indicate one player's turn
#define BLUE_LED PC5 // Blue LED to indicate the other player's turn
#define BUZZER PD3 // Buzzer that goes off when time runs out

// Function Prototypes
void peripherals_init(void); // Set up the ports for the LEDs and buzzer