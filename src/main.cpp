#include <Arduino.h>
#include <avr_debugger.h>
#include <USART.h>
#include <LCD.h>
#include <INT0.h>

char text[MAX_TEXT];

int main() {

  #ifdef __DEBUG__
    dbg_start();
  #endif

  // initialize USART/LCD/INT0 controller
  USART_init();
  LCD_init();
  INT0_init();

  // clear LCD display
	LCD_command(1);

  // reset variables
	memset(text, 0, MAX_TEXT);

  while(1) {

  }
}

ISR(INT0_vect) {

}