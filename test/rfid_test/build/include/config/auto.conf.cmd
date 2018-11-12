deps_config := \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/app_trace/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/aws_iot/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/bt/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/driver/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/esp32/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/ethernet/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/fatfs/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/freemodbus/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/freertos/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/heap/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/http_server/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/libsodium/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/log/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/lwip/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/mbedtls/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/mdns/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/mqtt/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/nvs_flash/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/openssl/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/pthread/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/spi_flash/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/spiffs/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/vfs/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/paulbreugnot/Development/ESP32/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
