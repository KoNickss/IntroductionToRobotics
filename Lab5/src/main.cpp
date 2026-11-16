#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "LAB5";

constexpr gpio_num_t LED_GPIO = GPIO_NUM_18;

constexpr gpio_num_t BUTTON_GPIO = GPIO_NUM_21;


extern "C" void app_main(void) {
  // 1. Configure the LED output
  gpio_config_t led_config = {};
  led_config.pin_bit_mask = (1ULL << LED_GPIO);
  led_config.mode = GPIO_MODE_OUTPUT;
  led_config.pull_up_en = GPIO_PULLUP_DISABLE;
  led_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  led_config.intr_type = GPIO_INTR_DISABLE;
  ESP_ERROR_CHECK(gpio_config(&led_config));
  ESP_ERROR_CHECK(gpio_set_level(LED_GPIO, 0));
  // 2. Configure the button input
  gpio_config_t button_config = {};
  button_config.pin_bit_mask = (1ULL << BUTTON_GPIO);
  button_config.mode = GPIO_MODE_INPUT;
  button_config.pull_up_en = GPIO_PULLUP_ENABLE;
  button_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  button_config.intr_type = GPIO_INTR_DISABLE;
  ESP_ERROR_CHECK(gpio_config(&button_config));
  ESP_LOGI(TAG, "GPIO initialized");
  // Remember the previous button state
  bool previousPressed = gpio_get_level(BUTTON_GPIO) == 0;
  while (true) {
    // Read button
    bool pressed = gpio_get_level(BUTTON_GPIO) == 0;
    // Control LED
    gpio_set_level(LED_GPIO, pressed ? 1 : 0);
    // Log only when the input changes
    if (pressed != previousPressed) {
      ESP_LOGI(TAG, "Button: %s", pressed ? "PRESSED" : "RELEASED");
      previousPressed = pressed;
    }
    // Poll every 20ms
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}