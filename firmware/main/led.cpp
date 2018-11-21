#include "led.h"
#include "FreeRTOS.h"

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GREEN_LED) | (1ULL<<YELLOW_LED) | (1ULL<<RED_LED))

void init_leds() {
  gpio_config_t io_conf;
  //disable interrupt
  io_conf.intr_type = (gpio_int_type_t) GPIO_PIN_INTR_DISABLE;
  //set as output mode
  io_conf.mode = GPIO_MODE_OUTPUT;
  //bit mask of the pins that you want to set,e.g.GPIO18/19
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  //disable pull-down mode
  io_conf.pull_down_en = (gpio_pulldown_t) 0;
  //disable pull-up mode
  io_conf.pull_up_en = (gpio_pullup_t) 0;
  //configure GPIO with the given settings
  gpio_config(&io_conf);

  switchOffAll();
}

void switchOffAll() {
  gpio_set_level((gpio_num_t) GREEN_LED, 0);
  gpio_set_level((gpio_num_t) YELLOW_LED, 0);
  gpio_set_level((gpio_num_t) RED_LED, 0);
}

Led::Led(int led) {
  this->led = (gpio_num_t) led;
}

void Led::on() {
  gpio_set_level(this->led, 1);
  status = 1;
}

void Led::off() {
  gpio_set_level(this->led, 0);
  status = 0;
}

void Led::toggle() {
  status ? off() : on();
}

void taskBlinkLed(void* blinkParameterPtr){
  BlinkParameter* blinkParameter = (BlinkParameter*) blinkParameterPtr;
  for (;;) {
    blinkParameter->led.toggle();
    vTaskDelay(blinkParameter->period / portTICK_RATE_MS);
  }
}
