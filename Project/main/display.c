#include "display.h"


void setup_display() {
    int center, top; 

    //Initialize the display (shared i2c) only once after boot.
    i2c_master_shared_i2c_init(&oled_display);

    //Uncomment this if you want to flip the display
    //dev._flip = true;

    ssd1306_init(&oled_display, 128, 64);
}

void update_display2(TimerHandle_t xTimer) {
    // ESP_LOGI(tag, "Writing a large Hello (notice the 2 different colours).");
    ssd1306_clear_screen(&oled_display, false);
    //ssd1306_contrast(&dev, 0xff); 
    ssd1306_display_text(&oled_display, 0, "TEST_1", 6, false);
    ssd1306_display_text(&oled_display, 1, "TEST_2", 6, false);
    ssd1306_display_text(&oled_display, 2, "TEST_3", 6, false);
    ssd1306_display_text(&oled_display, 3, "TEST_4", 6, false);
}