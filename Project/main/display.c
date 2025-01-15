#include "display.h"


SSD1306_t* setup_display() {
    int center, top; 
    SSD1306_t* oled_display = (SSD1306_t*) malloc(sizeof(SSD1306_t));
    vTaskDelay(pdMS_TO_TICKS(1000));
    //Initialize the display (shared i2c) only once after boot.
    i2c_master_shared_i2c_init(oled_display);

    //Uncomment this if you want to flip the display
    //dev._flip = true;

    ssd1306_init(oled_display, 128, 64);


    // TODO: Apparently you have to clear the screen twice, to no longer encounter the fragments.
    ssd1306_clear_screen(oled_display, false);
    //vTaskDelay(pdMS_TO_TICKS(1000));
    ssd1306_clear_screen(oled_display, false);
    //ssd1306_contrast(oled_display, 0xFF); 

    ssd1306_display_text(oled_display, 0, "TEST__1         ", 16, false);
    //for (int i = 1; i < 8; i++) {
     //   ssd1306_display_text(oled_display, i, "                ", 16, false);
    //}
    return oled_display;
}

void update_display2(TimerHandle_t xTimer) {
    /*
    // ESP_LOGI(tag, "Writing a large Hello (notice the 2 different colours).");
    ssd1306_clear_screen(&oled_display, false);
    //ssd1306_contrast(&dev, 0xff); 
    ssd1306_display_text(&oled_display, 0, "TEST_1", 6, false);
    ssd1306_display_text(&oled_display, 1, "TEST_2", 6, false);
    ssd1306_display_text(&oled_display, 2, "TEST_3", 6, false);
    ssd1306_display_text(&oled_display, 3, "TEST_4", 6, false);
*/
}

void shutdown_display(SSD1306_t* oled_display_ptr) {
    // TODO: ESP Log "shutting down" here..
    free(oled_display_ptr);
}