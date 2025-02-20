#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUZZ_TIMER              LEDC_TIMER_1
#define BUZZ_MODE               LEDC_LOW_SPEED_MODE
#define BUZZ_OUTPUT_IO          (9) // Define the output GPIO for red
#define BUZZ_CHANNEL            LEDC_CHANNEL_4
#define BUZZ_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define BUZZ_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define BUZZ_FREQUENCY          (1000) // Frequency in Hertz. Set frequency at 1 kHz

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO_RED      (4) // Define the output GPIO for red
#define LEDC_OUTPUT_IO_GREEN    (5) // Define the output GPIO for green
#define LEDC_OUTPUT_IO_BLUE     (6) // Define the output GPIO for blue
#define LEDC_CHANNEL_RED        LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN      LEDC_CHANNEL_1
#define LEDC_CHANNEL_BLUE       LEDC_CHANNEL_2
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (1000) // Frequency in Hertz. Set frequency at 1 kHz

// Note frequencies and rhythm taken from: https://www.hackster.io/slagestee/rickroll-box-3c2245 
#define  a3f    208     // 208 Hz
#define  b3f    233     // 233 Hz
#define  b3     247     // 247 Hz
#define  c4     261     // 261 Hz MIDDLE C
#define  c4s    277     // 277 Hz
#define  e4f    311     // 311 Hz    
#define  f4     349     // 349 Hz 
#define  a4f    415     // 415 Hz  
#define  b4f    466     // 466 Hz 
#define  b4     493     // 493 Hz 
#define  c5     523     // 523 Hz 
#define  c5s    554     // 554 Hz
#define  e5f    622     // 622 Hz  
#define  f5     698     // 698 Hz 
#define  f5s    740     // 740 Hz
#define  a5f    831     // 831 Hz 

#define rest    -1

int beatlengthInMs = 133; // determines tempo
float beatseparationconstant = 0.3;
int volumeInPercent = 25;

int song1_chorus_melody[] = { 
    b4f, b4f, a4f, a4f,
    f5, f5, e5f, b4f, b4f, a4f, a4f, e5f, e5f, c5s, c5, b4f,
    c5s, c5s, c5s, c5s,
    c5s, e5f, c5, b4f, a4f, a4f, a4f, e5f, c5s
};

int song1_chorus_rhythmn[] = { 
    1, 1, 1, 1,
    3, 3, 6, 1, 1, 1, 1, 3, 3, 3, 1, 2,
    1, 1, 1, 1,
    3, 3, 3, 1, 2, 2, 2, 4, 8
};

void setupBuzzer()
{
  ledc_timer_config_t ledc_timer_buzz = {
        .speed_mode       = BUZZ_MODE,
        .duty_resolution  = BUZZ_DUTY_RES,
        .timer_num        = BUZZ_TIMER,
        .freq_hz          = BUZZ_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_buzz));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel_buzz = {
        .speed_mode     = BUZZ_MODE,
        .channel        = BUZZ_CHANNEL,
        .timer_sel      = BUZZ_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = BUZZ_OUTPUT_IO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_buzz));
    ESP_ERROR_CHECK(ledc_set_duty(BUZZ_MODE, BUZZ_CHANNEL, 0));
    ESP_ERROR_CHECK(ledc_update_duty(BUZZ_MODE, BUZZ_CHANNEL));
}

void play(int notes[], int durations[], int notesSize, int durationsSize) {
    for(int i = 0; i < notesSize; i++) {
        if (notes[i] == -1) {
            ESP_ERROR_CHECK(ledc_set_duty(BUZZ_MODE, BUZZ_CHANNEL, 0));
            ESP_ERROR_CHECK(ledc_update_duty(BUZZ_MODE, BUZZ_CHANNEL));
        } else {
            ESP_ERROR_CHECK(ledc_set_duty(BUZZ_MODE, BUZZ_CHANNEL, 4096)); 
            ESP_ERROR_CHECK(ledc_update_duty(BUZZ_MODE, BUZZ_CHANNEL));
            ESP_ERROR_CHECK(ledc_set_freq(BUZZ_MODE, BUZZ_TIMER, notes[i])); 
        }
        vTaskDelay((durations[i] * beatlengthInMs) / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(ledc_set_duty(BUZZ_MODE, BUZZ_CHANNEL, 0));
        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(BUZZ_MODE, BUZZ_CHANNEL));
        vTaskDelay((beatseparationconstant * beatlengthInMs) / portTICK_PERIOD_MS);
    }
}

void rickroll() {
    play(song1_chorus_melody, song1_chorus_rhythmn, sizeof(song1_chorus_melody) / sizeof(int), sizeof(song1_chorus_rhythmn) / sizeof(int));
}
