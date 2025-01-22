#ifndef WIFI_TOOL
#define WIFI_TOOL

#include <stdint.h>
#include "esp_event.h"
#include "TaskSetup.h"

#define WIFI_SSID "Magnus"
#define WIFI_PASS "magnus123"
// #define SERVER_URL "https://172.20.10.2:5000/log"
#define SERVER_URL "http://172.20.10.2:5000/log"

static const char *TAG = "WIFI_CONNECT";

void setupWifi();
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void post_data(void *pvParameters);

#endif