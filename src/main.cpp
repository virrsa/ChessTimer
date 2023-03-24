#include <Arduino.h>
#include <avr_debugger.h>

int main() {
  #ifdef __DEBUG__
    dbg_start();
  #endif

  while(1) {

  }
}