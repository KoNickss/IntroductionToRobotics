#pragma once

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <cstdint>

namespace detail {

struct SongRequest {
  const char *rtttl;
  bool loop;
};

inline QueueHandle_t s_queue    = nullptr;
inline ledc_timer_t s_timer     = LEDC_TIMER_0;
inline ledc_channel_t s_channel = LEDC_CHANNEL_0;

inline void setTone(uint32_t freq_hz) {
  if (freq_hz == 0) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, s_channel, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, s_channel);
    return;
  }
  ledc_set_freq(LEDC_LOW_SPEED_MODE, s_timer, freq_hz);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, s_channel, 512);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, s_channel);
}

inline int parseInt(const char *&p) {
  int val = 0;
  while (*p >= '0' && *p <= '9') {
    val = val * 10 + (*p - '0');
    p++;
  }
  return val;
}

inline bool waitInterruptible(int ms, SongRequest &next_req) {
  if (ms <= 0) return false;
  TickType_t ticks = pdMS_TO_TICKS(ms);
  if (ticks == 0) ticks = 1;
  return xQueueReceive(s_queue, &next_req, ticks) == pdTRUE;
}

inline bool playRtttlOnce(const char *p, SongRequest &next_req) {
  constexpr uint32_t BASE_FREQ[12] = {
      262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494 // C4..B4
  };

  while (*p && *p != ':') p++; // scot titlu, intr o zi treaba asta o sa dea segfault simt deja in oase
  if (!*p) return false;
  p++;

  int def_dur = 4, def_oct = 6, bpm = 63;
  while (*p && *p != ':') {
    while (*p == ' ' || *p == ',') p++;
    if (*p == 'd' && *(p + 1) == '=') {
      p += 2;
      def_dur = parseInt(p);
    } else if (*p == 'o' && *(p + 1) == '=') {
      p += 2;
      def_oct = parseInt(p);
    } else if (*p == 'b' && *(p + 1) == '=') {
      p += 2;
      bpm = parseInt(p);
    } else {
      while (*p && *p != ',' && *p != ':') p++;
    }
  }
  if (!*p) return false;
  p++;

  if (bpm <= 0) bpm = 63;
  if (def_dur <= 0) def_dur = 4;
  int wholenote_ms = (60000 * 4) / bpm;

  while (*p) {
    while (*p == ' ' || *p == ',') p++;
    if (!*p) break;

    int dur = parseInt(p);
    if (dur <= 0) dur = def_dur;

    char c = *p;
    if (c >= 'A' && c <= 'Z') c += ('a' - 'A');
    if (c) p++;
    // multumesc chatgpt pentru ca m-ai salvat de a invata music theory
    int semitone = -1;
    switch (c) {
      case 'c': semitone = 0;  break;
      case 'd': semitone = 2;  break;
      case 'e': semitone = 4;  break;
      case 'f': semitone = 5;  break;
      case 'g': semitone = 7;  break;
      case 'a': semitone = 9;  break;
      case 'b':
      case 'h': semitone = 11; break;
      default:  semitone = -1; break;
    }

    int oct = def_oct;
    bool dotted = false;

    while (*p && *p != ',') {
      if (*p == '#') {
        if (semitone >= 0) {
          semitone++;
          if (semitone > 11) { semitone = 0; oct++; }
        }
        p++;
      } else if (*p == '.') {
        dotted = true;
        p++;
      } else if (*p >= '0' && *p <= '9') {
        oct = parseInt(p);
      } else {
        p++;
      }
    }

    int note_ms = wholenote_ms / dur;
    if (dotted) note_ms += note_ms / 2;

    uint32_t freq = 0;
    if (semitone >= 0) {
      freq = (oct >= 4) ? (BASE_FREQ[semitone] << (oct - 4))
                        : (BASE_FREQ[semitone] >> (4 - oct));
    }

    int play_ms = (note_ms > 15) ? (note_ms - 15) : note_ms;
    int gap_ms  = note_ms - play_ms;

    setTone(freq);
    if (waitInterruptible(play_ms, next_req)) {
      setTone(0);
      return true;
    }

    if (gap_ms > 0) {
      setTone(0);
      if (waitInterruptible(gap_ms, next_req)) {
        return true;
      }
    }
  }

  setTone(0);
  return false;
}

inline void buzzerTask(void *) {
  SongRequest current{nullptr, false};
  SongRequest next{nullptr, false};
  bool has_pending = false;

  while (true) {
    if (has_pending) {
      current = next;
      has_pending = false;
    } else {
      xQueueReceive(s_queue, &current, portMAX_DELAY);
    }

    if (!current.rtttl || !current.rtttl[0]) {
      setTone(0);
      continue;
    }

    do {
      if (playRtttlOnce(current.rtttl, next)) {
        has_pending = true;
        break;
      }
    } while (current.loop);
  }
}

}

inline void buzzerInit(gpio_num_t pin,
                       ledc_timer_t timer = LEDC_TIMER_0,
                       ledc_channel_t channel = LEDC_CHANNEL_0) {
  if (detail::s_queue != nullptr) return;

  detail::s_timer   = timer;
  detail::s_channel = channel;

  ledc_timer_config_t t_cfg = {};
  t_cfg.speed_mode      = LEDC_LOW_SPEED_MODE;
  t_cfg.duty_resolution = LEDC_TIMER_10_BIT;
  t_cfg.timer_num       = timer;
  t_cfg.freq_hz         = 1000;
  t_cfg.clk_cfg         = LEDC_AUTO_CLK;
  ESP_ERROR_CHECK(ledc_timer_config(&t_cfg));

  ledc_channel_config_t c_cfg = {};
  c_cfg.gpio_num   = pin;
  c_cfg.speed_mode = LEDC_LOW_SPEED_MODE;
  c_cfg.channel    = channel;
  c_cfg.intr_type  = LEDC_INTR_DISABLE;
  c_cfg.timer_sel  = timer;
  c_cfg.duty       = 0;
  c_cfg.hpoint     = 0;
  ESP_ERROR_CHECK(ledc_channel_config(&c_cfg));

  detail::s_queue = xQueueCreate(1, sizeof(detail::SongRequest));
  xTaskCreate(detail::buzzerTask, "rtttl_buzzer", 3072, nullptr, 3, nullptr);
}

inline void playThisSong(const char *rtttl, bool loop = false) {
  if (!detail::s_queue) return;
  detail::SongRequest req{rtttl, loop};
  xQueueOverwrite(detail::s_queue, &req);
}

inline void stopSong() {
  playThisSong(nullptr, false);
}