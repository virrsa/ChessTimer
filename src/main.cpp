#include <Arduino.h>
#include <avr_debugger.h>
#include <LCD.h>

int main() {
  
  #ifdef __DEBUG__
    dbg_start();
  #endif

  // initialize LCD controller
  LCD_init();

  // clear LCD display
	LCD_command(1);

  while(1) {

  }
}