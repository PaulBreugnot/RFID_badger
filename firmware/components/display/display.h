#include "u8g2.h"
#include "u8g2_esp32_hal.h"

u8g2_t init_display();
void init_header(u8g2_t u8g2);
void task_show_present_badge(void* u8g2_ptr);
