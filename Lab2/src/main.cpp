#include <Arduino.h>
#include <esp_arduino_version.h>
/*
void setup() {
	Serial.begin(115200);
	delay(500);
	Serial.println("=== ESP32 Information ===");
	Serial.print("Chip: ");
	Serial.println(ESP.getChipModel());
	Serial.print("CPU cores: ");
	Serial.println(ESP.getChipCores());
	Serial.print("CPU frequency: ");
	Serial.print(ESP.getCpuFreqMHz());
	Serial.println(" MHz");
	Serial.print("Flash size: ");
	Serial.print(ESP.getFlashChipSize() / (1024 * 1024));
	Serial.println(" MB");
	Serial.print("Free heap: ");
	Serial.print(ESP.getFreeHeap());
	Serial.println(" bytes");
	Serial.printf("Arduino-ESP32: %d.%d.%d\n",
					ESP_ARDUINO_VERSION_MAJOR,
					ESP_ARDUINO_VERSION_MINOR,
					ESP_ARDUINO_VERSION_PATCH);
	Serial.print("ESP-IDF: ");
	Serial.println(ESP.getSdkVersion());
}

void loop() {
	Serial.print("Uptime: ");
	Serial.print(millis());
	Serial.println(" ms");
	delay(1000);
}*/

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
#ifndef RED_LED_OUT_PIN 
#define RED_LED_OUT_PIN 18
#endif
void setup() {
	pinMode(RED_LED_OUT_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}
int led_out_value = 255;
int milis_for_fade_out = 0;
void loop() {
	// analogWrite(RED_LED_OUT_PIN, 0);
	// delay(400);
	// analogWrite(RED_LED_OUT_PIN, 10);
	// delay(400);
	// analogWrite(RED_LED_OUT_PIN, 64);
	// delay(400);
	// analogWrite(RED_LED_OUT_PIN, 128);
	// delay(400);
	// analogWrite(RED_LED_OUT_PIN, 192);
	// delay(400);
	// analogWrite(RED_LED_OUT_PIN, 255);
	// delay(400);
  int curr_milis = millis();
  if (curr_milis - 60 >= milis_for_fade_out){
    milis_for_fade_out = curr_milis;
    led_out_value /= 1.5;
    if (led_out_value <= 0 || led_out_value >= 255){
      led_out_value = 255;
    }
    analogWrite(RED_LED_OUT_PIN, led_out_value);
  }
}
