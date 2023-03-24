#include <Peripherals.h>

void peripherals_init(void) {
  DDRC |= (1 << GREEN_LED) | (1 << BLUE_LED);
  DDRD |= (1 << BUZZER);
}