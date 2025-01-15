#include "display.h"
#include "SensorTool.h"

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

}

void update_display(void* pvParameters) {
        struct SensorData* data = (struct SensorData*) pvParameters;
        char temperature_str[17];
        char humidity_str[17];
        char light_str[17];
        char soil_temp_str[17];
        char soil_moist_str[17];
    while(1) {
        snprintf(temperature_str,sizeof(temperature_str), "%-8s  %4.2fC", "Air:",data->air.temp);
        snprintf(humidity_str,sizeof(humidity_str), "%-8s  %4.2fC", "Humid.:", data->air.humid);
        snprintf(light_str,sizeof(light_str), "%-10s %4dL", "light:", data->light);
        snprintf(soil_temp_str,sizeof(soil_temp_str), "%-8s  %4.2fC", "S.Temp:", data->soil.temp);
        snprintf(soil_moist_str,sizeof(soil_temp_str), "%-9s  %5u", "S.Moist:", data->soil.moist);


        ssd1306_display_text(&oled_display, 0, "JOHN'S STATS:      ", 16, false);
        ssd1306_display_text(&oled_display, 3, temperature_str, sizeof(temperature_str), false);
        ssd1306_display_text(&oled_display, 4, humidity_str, sizeof(humidity_str), false);
        ssd1306_display_text(&oled_display, 5, light_str, sizeof(light_str), false);
        ssd1306_display_text(&oled_display, 6, soil_temp_str, sizeof(soil_temp_str), false);
        ssd1306_display_text(&oled_display, 7, soil_moist_str, sizeof(soil_moist_str), false);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}