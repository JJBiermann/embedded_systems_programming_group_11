#include "led_buzzer.h"

void setup_led()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY, // Set output frequency at 1 kHz
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel_red = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_RED,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LEDC_OUTPUT_IO_RED,
        .duty = 0, // Set duty to 0%
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_red));

    ledc_channel_config_t ledc_channel_green = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_GREEN,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LEDC_OUTPUT_IO_GREEN,
        .duty = 0, // Set duty to 0%
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_green));

    ledc_channel_config_t ledc_channel_blue = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_BLUE,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LEDC_OUTPUT_IO_BLUE,
        .duty = 0, // Set duty to 0%
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_blue));
}

void setup_buzzer()
{
    // Prepare and then apply the LEDC PWM timer configuration (we use it for the buzzer)
    ledc_timer_config_t ledc_timer_buzz = {
        .speed_mode = BUZZ_MODE,
        .duty_resolution = BUZZ_DUTY_RES,
        .timer_num = BUZZ_TIMER,
        .freq_hz = BUZZ_FREQUENCY, // Set output frequency at 1 kHz
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_buzz));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel_buzz = {
        .speed_mode = BUZZ_MODE,
        .channel = BUZZ_CHANNEL,
        .timer_sel = BUZZ_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = BUZZ_OUTPUT_IO,
        .duty = 0, // Set duty to 0%
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_buzz));
}

void led_green()
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, 8191);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, 8191);
}

void led_red()
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, 8191);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, 8191);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE);
}

void buzzer_on()
{
    ledc_set_duty(BUZZ_MODE, BUZZ_CHANNEL, 2048); // 50% duty, control volume
    ledc_update_duty(BUZZ_MODE, BUZZ_CHANNEL);
    ledc_set_freq(BUZZ_MODE, BUZZ_TIMER, 400);
}

void buzzer_off()
{
    ledc_set_duty(BUZZ_MODE, BUZZ_CHANNEL, 0); // 0% duty, control volume
    ledc_update_duty(BUZZ_MODE, BUZZ_CHANNEL);
}
