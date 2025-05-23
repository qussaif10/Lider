#include "lider_http.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <stdio.h>

#define HTTP_TAG "HTTP_CLIENT"

static const char* auth_header = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJiYTVmZjY1NTdlNzA0NmJiYmUwNWI0MGI4ZDlhNjJhNyIsImlhdCI6MTc0NzkwNTI1MywiZXhwIjoyMDYzMjY1MjUzfQ.IF4wLyNNEExUmAbW-Xw2dVOyUT5ALbCATtCnG_lO3kM";

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
       case HTTP_EVENT_ERROR:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            break;
    }

    return ESP_OK;
}

void lights_on(uint8_t brightness) {
    esp_http_client_config_t config = {
        .url = "http://10.1.1.44:31110/api/services/light/turn_on",
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_POST,
    };
    
    ESP_LOGI(HTTP_TAG, "Making HTTP request to home assistant to turn on lights");
    
    esp_http_client_handle_t client = esp_http_client_init(&config);

    char post_data[64];

    snprintf(post_data, sizeof(post_data), "{\"entity_id\":\"light.bedroom_lights\",\"brightness\":%u}", brightness);

    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    
    esp_http_client_set_header(client, "Authorization", auth_header);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    
    esp_err_t err = esp_http_client_perform(client);
    
    if (err == ESP_OK) {
        ESP_LOGI(HTTP_TAG, "HTTP POST Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(HTTP_TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
}



void lights_off() {
    esp_http_client_config_t config = {
        .url = "http://10.1.1.44:31110/api/services/light/turn_off",
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_POST,
    };
    
    ESP_LOGI(HTTP_TAG, "Making HTTP request to home assistant to turn off lights");
    
    esp_http_client_handle_t client = esp_http_client_init(&config);

    const char *post_data = "{\"entity_id\":\"light.bedroom_lights\"}";

    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    
    esp_http_client_set_header(client, "Authorization", auth_header);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    
    esp_err_t err = esp_http_client_perform(client);
    
    if (err == ESP_OK) {
        ESP_LOGI(HTTP_TAG, "HTTP POST Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(HTTP_TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
}
