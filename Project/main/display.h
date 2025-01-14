#ifndef DISPLAY
#define DISPLAY
#endif 

//Display libraies
#include "ssd1306.h"
#include "font8x8_basic.h"

static SSD1306_t oled_display;

void setup_display();
void update_display();

