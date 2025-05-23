idf_component_register(SRCS "lider_main.c" "lider_sensor.c" "lider_wifi.c" "lider_http.c" "lider.c"
                       PRIV_REQUIRES spi_flash esp_driver_gpio esp_timer esp_wifi esp_http_client nvs_flash
                       INCLUDE_DIRS "")
