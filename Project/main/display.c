#include "Display.h"
#include "SensorTool.h"
#include "freertos/queue.h"

void setup_display(void) {
    int center, top; 
    vTaskDelay(pdMS_TO_TICKS(1000));
    //Initialize the display (shared i2c) only once after boot.
    i2c_master_shared_i2c_init(&oled_display);

    //Uncomment this if you want to flip the display
    //dev._flip = true;

    ssd1306_init(&oled_display, 128, 64);

    // Note: Apparently you have to clear the screen twice, to no longer encounter the fragments.
    ssd1306_clear_screen(&oled_display, false);
    ssd1306_clear_screen(&oled_display, false);

    ssd1306_display_text(&oled_display, 0, "JOHN'S STATS:      ", 16, false);

}

void update_display(void* pvParameters) {
        
        // struct SensorData* data = (struct SensorData*) pvParameters;

        QueueHandle_t displayQueue = (QueueHandle_t) pvParameters;
        //struct SensorData* data = (struct SensorData*) malloc(sizeof(struct SensorData));
        //struct Message* msg = (struct Message*) malloc(sizeof(struct Message));
        struct Message msg;
        int light_data = 0; 
        char temperature_str[17];
        char humidity_str[17];
        char light_str[17];
        char soil_temp_str[17];
        char soil_moist_str[17];

    while(1) {

        if(xQueueReceive(displayQueue, &msg, (TickType_t) 1000) == pdTRUE) {
            switch (msg.mode)
            {
            case 'L':
                snprintf(light_str,sizeof(light_str), "%-10s %4dI", "light:", msg.sensorData.light);
                ssd1306_display_text(&oled_display, 5, light_str, sizeof(light_str), false);
                break;
            case 'A':
                snprintf(temperature_str,sizeof(temperature_str), "%-8s  %4.2fC", "Air:",msg.sensorData.air.temp);
                snprintf(humidity_str,sizeof(humidity_str), "%-8s  %4.2fC", "Humid.:", msg.sensorData.air.humid);
                ssd1306_display_text(&oled_display, 3, temperature_str, sizeof(temperature_str), false);
                ssd1306_display_text(&oled_display, 4, humidity_str, sizeof(humidity_str), false);
                break;
            case 'S':
                snprintf(soil_temp_str,sizeof(soil_temp_str), "%-8s  %4.2fC", "S.Temp:", msg.sensorData.soil.temp);
                snprintf(soil_moist_str,sizeof(soil_temp_str), "%-9s  %5u", "S.Moist:", msg.sensorData.soil.moist);
                ssd1306_display_text(&oled_display, 6, soil_temp_str, sizeof(soil_temp_str), false);
                ssd1306_display_text(&oled_display, 7, soil_moist_str, sizeof(soil_moist_str), false);
                break;
            default:
                break;
            }
        } 
    }
}