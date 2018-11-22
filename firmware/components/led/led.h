#define GREEN_LED    16
#define YELLOW_LED    5
#define RED_LED    17

#define ON 0
#define OFF 1

#include "driver/gpio.h"

class Led {
private:
  gpio_num_t led;
  int status;

public:
  Led(int led);
  void on();
  void off();
  void toggle();
};

struct BlinkParameter {
  Led led;
  int period; // in ms
  BlinkParameter(Led led, int period) : led(led), period(period) { }
};

void init_leds();
void taskBlinkLed(void* blinkParameterPtr);
void switchOffAll();
