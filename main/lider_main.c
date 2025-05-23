#include "freertos/idf_additions.h"
#include "lider_sensor.h"
#include "lider_wifi.h"
#include "lider.h"

void app_main(void) {
    wifi_setup();
    init_ultrasonic_gpio();

    xTaskCreate(
        &lider_task,
        "LiderTask",
        8192,
        NULL,
        4,
        NULL
    );
}
