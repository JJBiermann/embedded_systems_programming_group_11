#include "TaskSetup.h"

void IRAM_ATTR buttonOneInterruptHandler(void* args) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = esp_timer_get_time();  // Get time in microseconds
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // TODO: What is this? 
    if (current_time - last_interrupt_time > 200000) {  // 200 ms debounce time
        if (fastPolling) {
            xTimerChangePeriodFromISR(light_poll, pdMS_TO_TICKS(1000), &xHigherPriorityTaskWoken);
            xTimerChangePeriodFromISR(temp_humid_poll, pdMS_TO_TICKS(1000), &xHigherPriorityTaskWoken);
            xTimerChangePeriodFromISR(soil_poll, pdMS_TO_TICKS(1000), &xHigherPriorityTaskWoken);
            fastPolling = 0;
        } else {
            xTimerChangePeriodFromISR(light_poll, pdMS_TO_TICKS(5000), &xHigherPriorityTaskWoken);
            xTimerChangePeriodFromISR(temp_humid_poll, pdMS_TO_TICKS(5000), &xHigherPriorityTaskWoken);
            xTimerChangePeriodFromISR(soil_poll, pdMS_TO_TICKS(5000), &xHigherPriorityTaskWoken);
            fastPolling = 1;
        }
    }
    last_interrupt_time = current_time;
}

void IRAM_ATTR buttonTwoInterruptHandler(void* args) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = esp_timer_get_time();  // Get time in microseconds
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
    int flag = 0;
    if (current_time - last_interrupt_time > 200000) {  // 200 ms debounce time
        flag = !flag;
        xQueueSendFromISR(RRQueue, &flag, &xHigherPriorityTaskWoken);
    }
    last_interrupt_time = current_time;
}

void soilPollCB(TimerHandle_t xTimer) {
    struct SoilData soil = soilPoll();

    #ifdef DISPLAY_ACTIVE
    struct Message* msg1 = (struct Message*) malloc(sizeof(struct Message));
    msg1->mode = 'S';
    msg1->sensorData.soil.temp = soil.temp;
    msg1->sensorData.soil.moist = soil.moist;
    if(xQueueSend(displayQueue, (void*) &msg1, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the soil sensor1!\n");
    }
    #endif

    #ifdef DASHBOARD_ACTIVE
    struct Message* msg2 = (struct Message*) malloc(sizeof(struct Message));
    msg2->mode = 'S';
    msg2->sensorData.soil.temp = soil.temp;
    msg2->sensorData.soil.moist = soil.moist;
    if(xQueueSend(dashboardQueue, (void*) &msg2, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the soil sensor2!\n");
    }
    #endif
}

void tempHumidPollCB(TimerHandle_t xTimer) {
    struct AirData air = tempHumidPoll();
    #ifdef DISPLAY_ACTIVE
    struct Message* msg1 = (struct Message*) malloc(sizeof(struct Message));
    msg1->mode = 'A';
    msg1->sensorData.air.valid = true;
    msg1->sensorData.air.temp = air.temp;
    msg1->sensorData.air.humid = air.humid;
    if(xQueueSend(displayQueue, (void*) &msg1, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the air sensor!\n");
    }
    #endif

    #ifdef RGB_ACTIVE
    struct Message* msg2 = (struct Message*) malloc(sizeof(struct Message));
    msg2->mode = 'A';
    msg2->sensorData.air.valid = true;
    msg2->sensorData.air.temp = air.temp;
    msg2->sensorData.air.humid = air.humid;
    if(xQueueSend(rgbQueue, (void*) &msg2, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the air sensor!\n");
    }
    #endif

    #ifdef DASHBOARD_ACTIVE
    struct Message* msg3 = (struct Message*) malloc(sizeof(struct Message));
    msg3->mode = 'A';
    msg3->sensorData.air.valid = true;
    msg3->sensorData.air.temp = air.temp;
    msg3->sensorData.air.humid = air.humid;
    if(xQueueSend(dashboardQueue, (void*) &msg3, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the air sensor!\n");
    }
    #endif
}

void lightPollCB(TimerHandle_t xTimer) {
    int light = lightPoll();

    #ifdef DISPLAY_ACTIVE
    struct Message* msg1 = (struct Message*) malloc(sizeof(struct Message));
    msg1->mode = 'L';
    msg1->sensorData.light = light;
    if(xQueueSend(displayQueue, (void*) &msg1, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the light sensor!\n");
    }
    #endif

    #ifdef DASHBOARD_ACTIVE
    struct Message* msg2 = (struct Message*) malloc(sizeof(struct Message));
    msg2->mode = 'L';
    msg2->sensorData.light = light;
    if(xQueueSend(dashboardQueue, (void*) &msg2, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the light sensor!\n");
    }
    #endif
}

void setupAirSensorAndQueue() {
    temp_humid_poll = xTimerCreate("temp_humid_poll", AIR_SENSOR_POLLING_DELAY, pdTRUE, (void *)0, tempHumidPollCB);
    xTimerStart(temp_humid_poll, pdMS_TO_TICKS(500));
}

void setupSoilSensorAndQueue() {
    soil_poll = xTimerCreate("soil_poll", SOIL_SENSOR_POLLING_DELAY, pdTRUE, (void *)0, soilPollCB);
    xTimerStart(soil_poll, pdMS_TO_TICKS(500));
}

void setupLightSensorAndQueue() {
    light_poll = xTimerCreate("light_poll", LIGHT_SENSOR_POLLING_DELAY, pdTRUE, (void *)0, lightPollCB);
    xTimerStart(light_poll, pdMS_TO_TICKS(500));
}

void setupDisplayAndQueue() {
    displayQueue = xQueueCreate(20, sizeof(struct Message*));
    if (displayQueue == NULL) {
        printf("Display queue creation went wrong!\n");
    } else {
        printf("Display queue creation was successful!\n");
    }
    setup_display();
    xTaskCreate(update_display, "display", 8192 * 2, (void*) displayQueue, 2, NULL);
}

void setupRGBAndQueue() {
    rgbQueue = xQueueCreate(20, sizeof(struct Message*));
    if (rgbQueue == NULL) {
        printf("Queue creation went wrong!\n");
    } else {
        printf("Queue creation was successful!\n");
    }
    xTaskCreate(update_rgbled, "rgbled", 8192 * 2, (void*) rgbQueue, 1, NULL);
}


void checkRRQueue() {
    int flag = 0;
    while(1) {
        if (xQueueReceive(RRQueue, &flag, (TickType_t) 10) == pdTRUE) {
            if (flag) {
                rickroll();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void setupRR() {
    RRQueue = xQueueCreate(5, sizeof(int));
    setupBuzzer();
    xTaskCreate(checkRRQueue, "RR", 2048, NULL, 8, NULL);
}

void setupQueuesAndTasks() {
    setupRR();
    #ifdef LIGHT_SENSOR_ACTIVE
    setupLightSensorAndQueue();
    #endif
    #ifdef SOIL_SENSOR_ACTIVE
    setupSoilSensorAndQueue();
    #endif
    #ifdef AIR_SENSOR_ACTIVE
    setupAirSensorAndQueue();
    #endif
    #ifdef DISPLAY_ACTIVE
    setupDisplayAndQueue();
    #endif
    #ifdef RGB_ACTIVE
    setupRGBAndQueue();
    #endif 
}