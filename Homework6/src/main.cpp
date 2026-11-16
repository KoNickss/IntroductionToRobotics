#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_random.h"
#include <cstdint>
#include <array>

#include "lautar.hpp"

static const char *TAG = "ELEVATOR";

constexpr gpio_num_t DS_GPIO   = GPIO_NUM_18;
constexpr gpio_num_t BUZZ_GPIO = GPIO_NUM_19;
constexpr gpio_num_t STCP_GPIO = GPIO_NUM_22;
constexpr gpio_num_t SHCP_GPIO = GPIO_NUM_23;

constexpr gpio_num_t BTN_GPIOS[3] = {GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_36};
constexpr gpio_num_t LED_GPIOS[3] = {GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27};

constexpr uint32_t TICK_MS       = 20;
constexpr int32_t DOOR_CLOSE_MS  = 4000;
constexpr int32_t DOOR_OPEN_MS = 4000;
constexpr int32_t FLOOR_MOVE_MS  = 7000;

constexpr const char *DING_SONG = "Ding:d=8,o=5,b=140:e6,2c6";


constexpr std::array<const char*, 7> ELEVATOR_SONGS = {
    "CantinaBand2:d=4,o=6,b=260:4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6.,8g#6,8a6,8g6,8a3,8f#6,8g6,8f#6,4f6.,8d6,4c4,4p,4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6,8g6,8g3,4g6,8g3,8f#6,4g6,8c7,4a#6,4a6,4g6.,4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6,8c7,8g3,4c7,8g3,8a6,4g6,4f6.,8d6,4c4,4p,2d6,2f6,2a6,2c7,4d#7,4d7,8g#6,4a6,8f6,4f4,4e4,4d4,4c4,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8g#7,4a7,8f7,4d4,8a6,8d7,4p,4a6,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8g#7,4a7,8g7,4d#4,4e6,2c7,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8g#7,4a7,8f7,4d4,8a6,8d7,4a3,4a6,8a#6,8d7,4f7,8b6,8d7,4f7,8g#7,4a7,8d7,8p,8d4,4d6.,8f6,8a#6,8d7,8g#6,4a6,8f6,4p,4e4,4d4,4c4,4f5,8c4,8g#5,8a#3,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8c4,8g#5,8a#3,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8c4,8g#5,8a#3,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8g#5,8f5,8g5,8f5,8g#5,8f5,8g5,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8d5,8g#5,8d5,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8d5,8g#5,8d5,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8d5,8g#5,8d5,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8g#5,8f5,4g#5.,8f5,8d4,2p.,8p,4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6.,8g#6,8a6,8g6,8a3,8f#6,8g6,8f#6,4f6.,8d6,4c4,4p,4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6,8g6,8g3,4g6,8g3,8f#6,4g6,8c7,4a#6,4a6,4g6.,4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6,8c7,8g3,4c7,8g3,8a6,4g6,4f6.,8d6,4c4,4p,2d6,2f6,2a6,2c7,4d#7,4d7,8g#6,4a6,8f6,4f4,4e4,4d4,4c4,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8g#7,4a7,8f7,4d4,8a6,8d7,4p,4a6,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8g#7,4a7,8g7,4d#4,4e6,2c7,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8a7,8a3,4a6,8d4,4a7,8f7,8g#7,4a7,8f7,4d4,8a6,8d7,4a3,4a6,8a#6,8d7,4f7,8b6,8d7,4f7,8g#7,4a7,8d7,8p,8d4,4d6.,8f6,8a#6,8d7,8g#6,4a6,8f6,4p,4e4,4d4,4c4,4f5,8c4,8g#5,8a#3,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8c4,8g#5,8a#3,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8c4,8g#5,8a#3,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8g#5,8f5,8g5,8f5,8g#5,8f5,8g5,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8d5,8g#5,8d5,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8d5,8g#5,8d5,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8d5,8g#5,8d5,8f5,4g5,8g3,8f5,8g#5,8f5,8g5,8f5,8g#5,8d5,4f5,8g#5,8f5,4g#5.,8f5,8d4,2p.,8p,4c6,8p,8e6,4g4,4g6,8c4,2g6,8g6,8e6,8c6,2f6,8c4,4g6,8a6,4f4,8p,8e4,4p,4d4,2e6,4g6.,8c6,8c4,4e6,8c6,4e6,4g6,2f6,8c4,4g6,8a6,4d4,8p,8f#4,4p,4a4,2a#6,4d7,8a#6,8c#7,8b3,8c#7,4a#6,4c#7,4a#6,2f6,8f6,4g#6,8d7,4d4,8p,8d4,4p,4f#4,8g4,4a#5,8d6,2g6,8c4,4e6,8c4,2e6,8f4,2f6.,8f6,8f4,2f6.,8f6,4c6,8e7,8e6,2d7,2c7,8g6,4a#7.,2a7,2g7,2f7,8p,8f7,4d4,2e7,2d7,2c7,16e6.,32c7,4a#7.,2a7,2g7,2f#7,8p,4d6.,2a#6,2d7,8d6,8c#7,4b6,8c#7,4d7.,8f6,8a6,4c7,8f6,4c#7,4d7.,8p,8d4,8p,8d7,4f#4,8f7,8a7,8d7,8f7,8a#6,8d7,8g6,8a#6,8e7,8g7,8c7,8e7,8a#6,8c7,8g6,8c7,8f6,8c7,4f6.,8c#7,4f6,4f7,2f6.,8g#6,8g6,8g#6,8g6,8g#6,8g6,8g#6,8g6,8g#6,4c#7,8g#6,4c#7,8g#6,8c#7,8c#5,8g#6,8a#6,8b6,8c7,8b6,8a#6,8g#4,8g#6,8g6,8g#6,8g6,8g#6,8g6,8g#6,8g6,8g#6,4c#7,8g#6,4c#7,8g#6,8a6,8a#6,8a6,4a#6,8f4,8a6,8a#6,8a6,8a#6,8a6,8a#6,8b6,8c7,8b4,8f#6,8g6,8g#6,8g6,8g#6,8g6,8g#6,8g6,8g#6,8g6,8g#6,4c#7,8g#6,4c#7,8g#6,8a6,8a#6,8a6,4a#6,8g3,8a6,8a#6,8b6,8c7,8g6,8e6,8c6,8e4,8c6,8c#6,8c6,8c#6,4a#5,8c#6,8e6,4c#6,8e6,8f6,8e6,8f6,8f#6,8g6,4c6,8g6,8g#6,8g6,8g#6,8a6,8a#6,8a6,8a#6,8b6,4c7,4p,4c7,4p,4c7,4p,4c7,4p,4c7,2e7,8f7,8c#7,8g#6,8f6,8e6,8d#6,8d6,8p,8g4,8g6,4c7.,8g6,4c7.,8b6,4c7.,8b6,2c7,8c#7,8e6,4c#7,8g#3,8e6,8c#7,8e6,8c#7,8e6,8c#7,8e6,8d#6,4c7.,8c#7,8e6,4c#7,8g#3,8e6,8c#7,8e6,8c#7,8e6,8c#7,8e6,8d#6,4c7.,8c#7,8e6,4c#7,8d#4,8e6,8c#7,8e6,8c#7,8e6,8c#7,8e6,8d#6,4c7,8b5,8a#5,8c#6,8f#6,8a#6,8c6,8e6,8g6,8c7,8c#7,4c#6,8c6,4c#6,2p,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,8a5,8f5,8d5,4p.,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,8a5,8f5,8d5,4p.,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,16a5,16a#5,8a5,8f5,8d5,4p,2a5.,8a5,8p,2a5.,8a5,4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6.,8g#6,8a6,8g6,8a3,8f#6,8g6,8f#6,4f6.,8d6,4c4,4p,4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6,8g6,8g3,4g6,8g3,8f#6,4g6,8c7,4a#6,4a6,4g6.,4a6,4d7,4a6,4d7,8a6,4d7,8a6,8a3,8g#6,4a6,8c7,8g3,4c7,8g3,8a6,4g6,4f6.,8d6,4c4,4p,2d6,2f6,2a6,2c7,4d#7,4d7,8g#6,4a6,8f6,4f4,4e4,4d4,4c4",
    "furelise:d=8,o=5,b=125:e6,d#6,e6,d#6,e6,b,d6,c6,4a,p,c,e,a,4b,p,e,g#,b,4c6,p,e,e6,d#6,e6,d#6,e6,b,d6,c6,4a,p,c,e,a,4b,p,e,c6,b,2a",
    "mountain:d=4,o=5,b=140:8e,8f#,8g,8a,8b,8g,4b,8a#,8f#,4a#,8a,8f,4a,8e,8f#,8g,8a,8b,8g,8b,8e6,8d6,8b,8g,8b,4d6",
    "joy:d=4,o=5,b=140:e,e,f,g,g,f,e,d,c,c,d,e,e,d,2d,e,e,f,g,g,f,e,d,c,c,d,e,d,c,2c",
    "nokia:d=4,o=5,b=180:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a",
    "tannenbaum:d=8,o=5,b=100:4d,g.,16g,4g.,a,b.,16b,4b.,b,a,b,4c6,4f#,4a,2g",
    "wishyou:d=4,o=5,b=160:d,g,8g,8a,8g,8f#,e,e,e,a,8a,8b,8a,8g,f#,d,d,b,8b,8c6,8b,8a,g,e,8d,8d,e,a,f#,2g."
};

const char* jukebox() {
    uint32_t idx = esp_random() % ELEVATOR_SONGS.size();
    return ELEVATOR_SONGS[idx];
}

constexpr const char *DOOR_CLOSE_SONG = "AstraIVA:d=4,o=5,b=140:g,2e";


constexpr uint8_t DIGIT_PATTERNS[10] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111
};

enum class ElevatorState {
  IDLE,
  CLOSING_DOORS,
  MOVING
};



static ElevatorState state = ElevatorState::IDLE;
static uint8_t current_floor = 1;
static bool going_up = true;
static bool requests[3] = {false, false, false};
static bool prev_btn[3] = {false, false, false};
static int32_t state_timer_ms = 0;

static void updateDisplay(void);

static void closeDoors(){
  state = ElevatorState::CLOSING_DOORS;
  ESP_LOGI(TAG, "CLOSING DOORS!");
  playThisSong(DOOR_CLOSE_SONG);
  state_timer_ms = DOOR_CLOSE_MS;
}

static void openDoors(){
  state = ElevatorState::IDLE;
  ESP_LOGI(TAG, "OPENING DOORS!");
  playThisSong(DING_SONG);
  updateDisplay();
  state_timer_ms = DOOR_OPEN_MS; //wait one second, dont start closing immediately
}

static void startMoving(){
  state = ElevatorState::MOVING;
  playThisSong(jukebox(), true);
}

static void shiftOutByte(uint8_t data) {
  gpio_set_level(STCP_GPIO, 0);

  for (int i = 7; i >= 0; i--) {
    gpio_set_level(SHCP_GPIO, 0);
    gpio_set_level(DS_GPIO, (data >> i) & 0x01);
    gpio_set_level(SHCP_GPIO, 1);
  }

  gpio_set_level(STCP_GPIO, 1);
  gpio_set_level(STCP_GPIO, 0);
}

void setDisplay(int8_t value) {
  if (value < 0 || value > 19) {
    return;
  }

  bool dot = value > 9;
  uint8_t digit = dot ? (value - 10) : value;

  if (digit > 9) {
    return;
  }

  uint8_t pattern = DIGIT_PATTERNS[digit] | (dot ? (1 << 7) : 0);
  shiftOutByte(pattern);
}

static void updateDisplay(void) {
  int8_t encoded = (state != ElevatorState::MOVING) ? (current_floor + 10) : current_floor;
  setDisplay(encoded);
}

static void setRequest(uint8_t floor_idx, bool active) {
  requests[floor_idx] = active;
  gpio_set_level(LED_GPIOS[floor_idx], active ? 1 : 0);
}

static bool hasAnyRequest(void) {
  return requests[0] || requests[1] || requests[2];
}

static bool hasRequestAbove(uint8_t floor) {
  for (uint8_t f = floor + 1; f <= 3; f++) {
    if (requests[f - 1]) {
      return true;
    }
  }
  return false;
}

static bool hasRequestBelow(uint8_t floor) {
  for (uint8_t f = 1; f < floor; f++) {
    if (requests[f - 1]) {
      return true;
    }
  }
  return false;
}

static void elevatorTickCallback(void *arg) {
  for (uint8_t i = 0; i < 3; i++) {
    bool pressed = gpio_get_level(BTN_GPIOS[i]) == 1;
    if (pressed && !prev_btn[i]) {
      setRequest(i, true);
      ESP_LOGI(TAG, "Request added: Floor %u", i + 1);
    }
    prev_btn[i] = pressed;
  }

  switch (state) {
    case ElevatorState::IDLE: {

      state_timer_ms -= TICK_MS;
      if (state_timer_ms > 0) {
        break;
      }

      if (!hasAnyRequest()) {
        break;
      }

      if (requests[current_floor - 1]) {
        closeDoors();
        
      } else if (going_up) {
        if (!hasRequestAbove(current_floor)) {
          going_up = false;
        }
        closeDoors();
        
      } else {
        if (!hasRequestBelow(current_floor)) {
          going_up = true;
        }
        closeDoors();
        
      }

      updateDisplay();
      break;
    }

    case ElevatorState::CLOSING_DOORS: {
      state_timer_ms -= TICK_MS;
      if (state_timer_ms > 0) {
        break;
      }

      if (requests[current_floor - 1]) {
        setRequest(current_floor - 1, false);
        ESP_LOGI(TAG, "Serviced Floor %u", current_floor);

        if (going_up && !hasRequestAbove(current_floor) && hasRequestBelow(current_floor)) {
          going_up = false;
        } else if (!going_up && !hasRequestBelow(current_floor) && hasRequestAbove(current_floor)) {
          going_up = true;
        }

        if (!hasAnyRequest()) {
          openDoors();
        } else {
          startMoving();
          state_timer_ms = FLOOR_MOVE_MS;
          updateDisplay();
        }
      } else {
        if (going_up && hasRequestAbove(current_floor)) {
          startMoving();
          state_timer_ms = FLOOR_MOVE_MS;
          updateDisplay();
        } else if (!going_up && hasRequestBelow(current_floor)) {
          startMoving();
          state_timer_ms = FLOOR_MOVE_MS;
          updateDisplay();
        } else if (hasRequestAbove(current_floor)) {
          going_up = true;
          startMoving();
          state_timer_ms = FLOOR_MOVE_MS;
          updateDisplay();
        } else if (hasRequestBelow(current_floor)) {
          going_up = false;
          startMoving();
          state_timer_ms = FLOOR_MOVE_MS;
          updateDisplay();
        } else {
          openDoors();
        }
      }
      break;
    }

    case ElevatorState::MOVING: {
      state_timer_ms -= TICK_MS;
      if (state_timer_ms > 0) {
        break;
      }

      current_floor += going_up ? 1 : -1;
      ESP_LOGI(TAG, "Arrived at Floor %u (%s)", current_floor, going_up ? "UP" : "DOWN");

      if (requests[current_floor - 1]) {
        setRequest(current_floor - 1, false);
        ESP_LOGI(TAG, "Serviced Floor %u", current_floor);

        if (going_up && !hasRequestAbove(current_floor) && hasRequestBelow(current_floor)) {
          going_up = false;
        } else if (!going_up && !hasRequestBelow(current_floor) && hasRequestAbove(current_floor)) {
          going_up = true;
        }

        openDoors();

        updateDisplay();

        // if (!hasAnyRequest()) {
        //   openDoors();
        // } else {
        //   closeDoors();
          
        // }
      } else {
        updateDisplay();
        state_timer_ms = FLOOR_MOVE_MS;
      }
      break;
    }
  }
}

extern "C" void app_main(void) {
  gpio_config_t out_config = {};
  out_config.pin_bit_mask = (1ULL << DS_GPIO) | (1ULL << STCP_GPIO) | (1ULL << SHCP_GPIO) | (1ULL << BUZZ_GPIO) |
                            (1ULL << LED_GPIOS[0]) | (1ULL << LED_GPIOS[1]) | (1ULL << LED_GPIOS[2]);
  out_config.mode = GPIO_MODE_OUTPUT;
  out_config.pull_up_en = GPIO_PULLUP_DISABLE;
  out_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  out_config.intr_type = GPIO_INTR_DISABLE;
  ESP_ERROR_CHECK(gpio_config(&out_config));

  gpio_config_t in_config = {};
  in_config.pin_bit_mask = (1ULL << BTN_GPIOS[0]) | (1ULL << BTN_GPIOS[1]) | (1ULL << BTN_GPIOS[2]);
  in_config.mode = GPIO_MODE_INPUT;
  in_config.pull_up_en = GPIO_PULLUP_DISABLE;
  in_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  in_config.intr_type = GPIO_INTR_DISABLE;
  ESP_ERROR_CHECK(gpio_config(&in_config));

  gpio_set_level(DS_GPIO, 0);
  gpio_set_level(STCP_GPIO, 0);
  gpio_set_level(SHCP_GPIO, 0);
  for (uint8_t i = 0; i < 3; i++) {
    gpio_set_level(LED_GPIOS[i], 0);
  }

  updateDisplay();

  buzzerInit(BUZZ_GPIO);

  esp_timer_create_args_t timer_args = {};
  timer_args.callback = &elevatorTickCallback;
  timer_args.name = "elevator_tick";

  esp_timer_handle_t elevator_timer;
  ESP_ERROR_CHECK(esp_timer_create(&timer_args, &elevator_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(elevator_timer, TICK_MS * 1000));
}