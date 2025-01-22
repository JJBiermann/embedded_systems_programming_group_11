#include <stdint.h>
#include "esp_event.h"

#define WIFI_SSID "Magnus"
#define WIFI_PASS "magnus123"
// #define SERVER_URL "https://172.20.10.2:5000/log"
#define SERVER_URL "http://172.20.10.2:5000/log"

static const char *TAG = "WIFI_CONNECT";

void setupWifi();
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void post_data(void *pvParameters);

extern const uint8_t server_cert_pem_start[] asm("_binary_server_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_server_cert_pem_end");
extern const uint8_t client_cert_pem_start[] asm("_binary_client_cert_pem_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_cert_pem_end");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_pem_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_pem_end");