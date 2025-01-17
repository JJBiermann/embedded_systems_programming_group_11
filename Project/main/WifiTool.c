#include "WifiTool.h"
#include "SensorTool.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "cJSON.h"



static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Connecting to Wi-Fi...");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Disconnected from Wi-Fi. Reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Connected! Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void setupWifi(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void postData(void *pvParameters) {
    struct SensorData* data = (struct SensorData*) pvParameters;

    while (1) {
        esp_http_client_config_t config = {
            .url = SERVER_URL,
            .timeout_ms = 10000,
        };
        esp_http_client_handle_t client = esp_http_client_init(&config);

        cJSON *root = cJSON_CreateObject();
        
        cJSON_AddNumberToObject(root, "light", data->light); 
        cJSON_AddNumberToObject(root, "air_temp", data->air.temp);  
        cJSON_AddNumberToObject(root, "air_humid", data->air.humid); 
        cJSON_AddNumberToObject(root, "soil_temp", data->soil.temp); 
        cJSON_AddNumberToObject(root, "soil_moist", data->soil.moist); 
        
        const char *post_data = cJSON_Print(root);

        esp_http_client_set_method(client, HTTP_METHOD_POST);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, post_data, strlen(post_data));

        ESP_LOGI(TAG, "Sending HTTP POST request to %s", SERVER_URL);
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
                     esp_http_client_get_status_code(client),
                     esp_http_client_get_content_length(client));
        } else {
            ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        }

        esp_http_client_cleanup(client);
        cJSON_Delete(root);
        free((void *)post_data);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
