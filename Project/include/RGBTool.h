#ifndef RGBTOOL_H
#define RGBTOOL_H
#include "driver/ledc.h"

#include "PollConfig.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO_RED      (4) // Define the output GPIO for red
#define LEDC_OUTPUT_IO_GREEN    (5) // Define the output GPIO for green
#define LEDC_OUTPUT_IO_BLUE     (6) // Define the output GPIO for blue
#define LEDC_CHANNEL_RED        LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN      LEDC_CHANNEL_1
#define LEDC_CHANNEL_BLUE       LEDC_CHANNEL_2
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY          (1000) // Frequency in Hertz. Set frequency at 1 kHz

#define DUTY_MAX 8192
#define RGB_MAX 255
#define RGB_MIN 0
#define RGB_RANGE (RGB_MAX - RGB_MIN + 1)

#define RGB_GREEN (struct RGB){ .r = 0, .g = 255, .b = 0 }
#define RGB_YELLOW (struct RGB){ .r = 255, .g = 255, .b = 0 }
#define RGB_RED (struct RGB){ .r = 255, .g = 0, .b = 0 }
#define RGB_BLUE (struct RGB){ .r = 0, .g = 0, .b = 255 }
#define RGB_LIGHT_BLUE (struct RGB){ .r = 0, .g = 0, .b = 128 }


struct RGB {
    int r;
    int g;
    int b;
};

void setupLED();
void setRGB(struct RGB);
void update_rgbled(void* pvParameters);


#endif