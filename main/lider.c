#include "freertos/idf_additions.h"
#include "lider_http.h"
#include "lider_sensor.h"
#include <stdint.h>
#include <math.h>

#define DELAY_MS 700

int distance_to_brightness(uint8_t *brightness, float distance) {
    if (distance <= 10.0f) {
        *brightness = 0;
        return 0;
    } else if (distance >= 30.0f && distance <= 40.0f) {
        *brightness = 255;
        return 0;
    } else if (distance > 40.0f) {
        return 1;
    } else {
        float scaled = (distance - 10.0f) / 20.0f * 255.0f;

        *brightness = (uint8_t)roundf(scaled);
        return 0;
    }
}

void lider_task(void *pvParameters) {
    while (1) {
        float distance = 0.0f;
        int result = get_distance(&distance);

        if (result != 0) {
            continue;
        }

        uint8_t brightness = 0;

        int maxed = distance_to_brightness(&brightness, distance);

        if (maxed == 1) {
            vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
            continue;
        }

        if (brightness != 0) {
            lights_on(brightness);
        } else {
            lights_off();
        }

        vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
    }
}
