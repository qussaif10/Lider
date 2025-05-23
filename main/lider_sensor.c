#include "lider_sensor.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "portmacro.h"

#define TRIG_GPIO GPIO_NUM_5
#define ECHO_GPIO GPIO_NUM_4
#define HC_TAG "HC-SR04"

volatile int64_t echo_start = 0;
volatile int64_t echo_end = 0;
volatile bool echo_done = false;

static void IRAM_ATTR echo_isr_handler(void* arg) {
    if (gpio_get_level(ECHO_GPIO) == 1) {
        echo_start = esp_timer_get_time();
    } else {
        echo_end = esp_timer_get_time();
        echo_done = true;
    }
}

void init_ultrasonic_gpio(void) {
    gpio_config_t trig_conf = {
        .pin_bit_mask = 1ULL << TRIG_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&trig_conf);
    gpio_set_level(TRIG_GPIO, 0);
    
    gpio_config_t echo_conf = {
        .pin_bit_mask = 1ULL << ECHO_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&echo_conf);
    
    gpio_install_isr_service(0);
    gpio_isr_handler_add(ECHO_GPIO, echo_isr_handler, NULL);
    
    ESP_LOGI(HC_TAG, "Ultrasonic GPIO initialized");
}

int get_distance(float *distance) {
    ESP_LOGI(HC_TAG, "Measuring distance");
    
    echo_done = false;
    
    gpio_set_level(TRIG_GPIO, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIG_GPIO, 0);
    
    int64_t wait_start = esp_timer_get_time();
    while (!echo_done && (esp_timer_get_time() - wait_start) < 30000) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    if (echo_done) {
        int64_t pulse_duration = echo_end - echo_start;
        float distance_cm = pulse_duration / 58.0f;
        ESP_LOGI(HC_TAG, "Distance: %.2f cm", distance_cm);

        *distance = distance_cm;
        return 0;
    } else {
        ESP_LOGW(HC_TAG, "Timeout waiting for echo");
        return 1;
    }
}
