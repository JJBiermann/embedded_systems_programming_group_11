#include "display.h"


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

//TODO: Needs data args
void update_display(TimerHandle_t xTimer) {
    //for (int i = 1; i < 8; i++) {
     //   ssd1306_display_text(oled_display, i, "                ", 16, false);
    //}
    // ESP_LOGI(tag, "Writing a large Hello (notice the 2 different colours).");
    ssd1306_clear_screen(&oled_display, false);
    ssd1306_clear_screen(&oled_display, false);
    //ssd1306_contrast(&dev, 0xff); 
    ssd1306_display_text(&oled_display, 0, "TEST_1", 6, false);
    ssd1306_display_text(&oled_display, 1, "TEST_2", 6, false);
    ssd1306_display_text(&oled_display, 2, "TEST_3", 6, false);
    ssd1306_display_text(&oled_display, 3, "TEST_4", 6, false);
}