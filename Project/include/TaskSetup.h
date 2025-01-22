#ifndef TASK_SETUP
#define TASK_SETUP

#include "esp_timer.h"

#include "SensorTool.h"
#include "RickRollBuzzer.h"
#include "RGBTool.h"
#include "Display.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"
#include "CustomConfig.h"

static QueueHandle_t displayQueue;
static QueueHandle_t rgbQueue;
static QueueHandle_t dashboardQueue;

static QueueHandle_t RRQueue;

static int fastPolling = 1;

static TimerHandle_t light_poll, temp_humid_poll, soil_poll;
//TaskHandle_t display_task_handle = NULL;
//TaskHandle_t rgb_task_handle = NULL;

//TaskHandle_t dashboard_task_handle = NULL;

void setupQueuesAndTasks();
void setupAirSensorAndQueue();
void setupSoilSensorAndQueue();
void setupLightSensorAndQueue();
void setupDisplayAndQueue();
void setupRGBAndQueue();
void buttonOneInterruptHandler(void* args);
void buttonTwoInterruptHandler(void* args);

#endif