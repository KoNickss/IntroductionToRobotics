#include "esp32-hal.h"
#include <Arduino.h>
const int buttonPin = 21;
const int ledPin = 18;
const int builtinLed = 2;
const unsigned long debounceDelay = 30;
int lastReading = HIGH;
int buttonState = HIGH;
unsigned long lastDebounceTime = 0;
bool ledState = LOW;
unsigned int buttonPushCounter = 0;


typedef enum {
	MODE_OFF,
	MODE_DIM,
	MODE_FULL,
	MODE_BLINK
}LampMode;


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  Serial.begin(115200);
  // Initialize the states from the actual input.
  lastReading = digitalRead(buttonPin);
  buttonState = lastReading;
  lastDebounceTime = millis();
}



void blinkMe(int pin){

	static int milis_for_fade_out = 0;
	static int led_out_value = 255;

	int curr_milis = millis();
  if (curr_milis - 60 >= milis_for_fade_out){
    milis_for_fade_out = curr_milis;
    led_out_value /= 1.5;
    if (led_out_value <= 0 || led_out_value >= 255){
      led_out_value = 255;
      
    }
    analogWrite(pin, led_out_value);
  }
}

void blinkMe2(int pin){

	static int milis_for_fade_out = 0;
	static int led_out_value = 255;

	int curr_milis = millis();
  if (curr_milis - 60 >= milis_for_fade_out){
    milis_for_fade_out = curr_milis;
    led_out_value /= 1.5;
    if (led_out_value <= 0 || led_out_value >= 255){
      led_out_value = 255;
      
    }
    analogWrite(pin, led_out_value);
  }
}


void actionLedAccordingToLampState(LampMode lampmode, int pin){

	static LampMode lastLampMode = MODE_OFF;

	if(lampmode == MODE_BLINK)
		blinkMe2(pin);
	else if(lampmode != lastLampMode){
		switch(lampmode){
			case MODE_OFF:
				analogWrite(pin, 0);
				break;
			case MODE_DIM:
				analogWrite(pin, 64);
				break;
			case MODE_FULL:
				analogWrite(pin, 255);
				break;

			default:
				break;
		}

		lastLampMode = lampmode;
	}
}

LampMode LampModeInc(LampMode l){
	switch(l){
		case MODE_OFF:
			return MODE_DIM;
		case MODE_DIM:
			return MODE_FULL;
		case MODE_FULL:
			return MODE_BLINK;
	}

	return MODE_OFF;
}

LampMode GLOBAL_LAMP_MODE = MODE_OFF;


void loop() {

	blinkMe(builtinLed);

	actionLedAccordingToLampState(GLOBAL_LAMP_MODE, ledPin);

  unsigned long currentTime = millis();
  int reading = digitalRead(buttonPin);
  // Reset the timer whenever the raw input changes.
  if (reading != lastReading) {
    lastDebounceTime = currentTime;
  }
  // Accept a new state only after it remains stable.
  if (currentTime - lastDebounceTime >= debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      // Count only confirmed button presses.
      if (buttonState == LOW) {
        GLOBAL_LAMP_MODE = LampModeInc(GLOBAL_LAMP_MODE);
				Serial.println(GLOBAL_LAMP_MODE);
        ledState = !ledState;
      }
    }
  }
  lastReading = reading;
}