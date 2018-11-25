#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "display.h"
#include "esp_log.h"

// SDA - GPIO21
#define PIN_SDA 21

// SCL - GPIO22
#define PIN_SCL 22

static const char *TAG = "ssd1306";

u8g2_t init_display() {
	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda   = PIN_SDA;
	u8g2_esp32_hal.scl  = PIN_SCL;
	u8g2_esp32_hal_init(u8g2_esp32_hal);


	u8g2_t u8g2; // a structure which will contain all the data for one display
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(
		&u8g2,
		U8G2_R0,
		//u8x8_byte_sw_i2c,
		u8g2_esp32_i2c_byte_cb,
		u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
	u8x8_SetI2CAddress(&u8g2.u8x8,0x78);

	ESP_LOGI(TAG, "u8g2_InitDisplay");
	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

	ESP_LOGI(TAG, "u8g2_SetPowerSave");
	u8g2_SetPowerSave(&u8g2, 0); // wake up display
	ESP_LOGI(TAG, "u8g2_ClearBuffer");
	u8g2_ClearBuffer(&u8g2);

	set_header(&u8g2, "Min'Bot Inc.");

	return u8g2;
}

void set_header(u8g2_t* u8g2, char* header){
	clear_header(u8g2);
	// u8g2_SetFont(u8g2, u8g2_font_crox2cb_tf);
	u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga9_t_all);
	u8g2_DrawUTF8(u8g2, 2, 15, header);
	u8g2_DrawHLine(u8g2, 0, 16, 128);
	u8g2_SendBuffer(u8g2);
}

void task_show_present_badge(void* animationParameter_ptr) {

	struct AnimationParameter* animationParameter = (struct AnimationParameter*) animationParameter_ptr;
	u8g2_t* u8g2 = animationParameter->u8g2;

  u8g2_SetFont(u8g2, u8g2_font_mercutio_basic_nbp_t_all);
  u8g2_DrawUTF8(u8g2, 13,30,"Présenter un badge");
	int arrow_positions[] = {0, 2, 4, 6, 4, 2};
	int i = 0;
	// u8g2_ClearBuffer(u8g2);
	while (*animationParameter->run) {
	// u8g2_ClearBuffer(u8g2);
	u8g2_SetDrawColor(u8g2, 0);
	u8g2_DrawBox(u8g2, 55, 32, 18, 63);
	u8g2_SetDrawColor(u8g2, 1);
	u8g2_DrawBox(u8g2, 60, 32 + arrow_positions[i], 8, 15);
	u8g2_DrawTriangle(u8g2, 55, 47 + arrow_positions[i], 73, 47 + arrow_positions[i], 64, 57 + arrow_positions[i]);
	u8g2_SendBuffer(u8g2);
	vTaskDelay(10 / portTICK_RATE_MS);
	i++;
	if (i > 5) {
		i = 0;
	}
}
clear_body(u8g2);
vTaskDelete(NULL);
}

void show_correct_rfid(u8g2_t* u8g2) {
	u8g2_DrawXBMP(u8g2, 39, 17, tick_width, tick_height, tick_bits);
	set_header(u8g2, "Bienvenue!"); // SendBuffer included in set_header
}

void show_wrong_rfid(u8g2_t* u8g2) {
	clear_body(u8g2);
  u8g2_DrawXBMP(u8g2, 39, 18, cross_width, cross_height, cross_bits);
	set_header(u8g2, "Accès refusé"); // SendBuffer included in set_header
}

void show_welcome(u8g2_t* u8g2, char* label) {
	clear_body(u8g2);
	u8g2_SetFont(u8g2, u8g2_font_ncenR08_tf);
	u8g2_DrawStr(u8g2, 10, 35, "Utilisateur :");
	u8g2_DrawStr(u8g2, 10, 49, label);
	u8g2_DrawStr(u8g2, 10, 64, "Status : ADMIN");

	u8g2_SendBuffer(u8g2);
}

void show_warning(u8g2_t* u8g2) {
	clear_header(u8g2);
	u8g2_SetFont(u8g2, u8g2_font_crox2cb_tf);
	u8g2_DrawXBMP(u8g2, 0, 0, warning_width, warning_height, warning_bits);
	u8g2_DrawXBMP(u8g2, 113, 0, warning_width, warning_height, warning_bits);
  u8g2_DrawStr(u8g2, 18, 15, "Attention");

	u8g2_SendBuffer(u8g2);
}


void show_door_unlocked(u8g2_t* u8g2) {
	clear_body(u8g2);

	u8g2_SetFont(u8g2, u8g2_font_mercutio_basic_nbp_t_all);
  u8g2_DrawUTF8(u8g2, 25,40,"Porte ouverte !");
	u8g2_DrawXBMP(u8g2, 0, 22, unlock_width, unlock_height, unlock_bits);
	u8g2_DrawXBMP(u8g2, 107, 22, unlock_width, unlock_height, unlock_bits);
	u8g2_DrawUTF8(u8g2, 20,58,"Pensez  à  badger.");

	u8g2_SendBuffer(u8g2);
}

void show_door_lock(u8g2_t* u8g2) {
	clear_body(u8g2);

	set_header(u8g2, "Au revoir !");
	u8g2_SetFont(u8g2, u8g2_font_mercutio_basic_nbp_t_all);
  u8g2_DrawUTF8(u8g2, 25,30,"Fermeture . . .");
	u8g2_DrawXBMP(u8g2, 49, 35, lock_width, lock_height, lock_bits);

	u8g2_SendBuffer(u8g2);
}

void clear_header(u8g2_t* u8g2) {
	u8g2_SetDrawColor(u8g2, 0);
	u8g2_DrawBox(u8g2, 0, 0, 128, 16);
	u8g2_SetDrawColor(u8g2, 1);
	u8g2_SendBuffer(u8g2);
}

void clear_body(u8g2_t* u8g2) {
	u8g2_SetDrawColor(u8g2, 0);
	u8g2_DrawBox(u8g2, 0, 17, 128, 47);
	u8g2_SetDrawColor(u8g2, 1);
	u8g2_SendBuffer(u8g2);
}
