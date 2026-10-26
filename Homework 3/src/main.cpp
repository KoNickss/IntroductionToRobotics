#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include <Arduino.h>
// #include "pitches.h"


#define LED_BUILTIN 2
#define EXTERNAL_LED 18
#define PMETER 34
#define ALED_R 25
#define ALED_G 26
#define ALED_B 27
#define PASS_BUZZ 23

#define THIRD_POTENTIO_SEG 1366


void setup() {
	pinMode(PMETER, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PASS_BUZZ, OUTPUT);
  pinMode(ALED_R, OUTPUT);
  pinMode(ALED_G, OUTPUT);
  pinMode(ALED_B, OUTPUT);
  pinMode(EXTERNAL_LED, OUTPUT);
  Serial.begin(115200);


  tone(PASS_BUZZ, 2000, 300);
}


int led_out_value = 255;

int external_led_out_value = 0;

int milis_for_fade_out = 0;

typedef enum{null, low, mid, high, HOLYSHIT} intenstate;

intenstate ist = null;

intenstate checkIntensity(int pin){
  int val = analogRead(pin);

  switch(val/THIRD_POTENTIO_SEG){
    case 0:
      return low;
      break;
    case 1:
      return mid;
      break;
    case 2:
      return high;
      break;
    default:
      return HOLYSHIT;
      break;
  }

  return null;
}

String intenstateToStr(intenstate i){
  switch(i){
    case low:
      return "low";
      break;
    case mid:
      return "mid";
      break;
    case high:
      return "high";
      break;
    case null:
      return "null";
      break;
    case HOLYSHIT:
      return "HOLYSHIT";
      break;
    default:
      return "null";
      break;
  }

  return "null";
}


void setLedColourToInten(int pinr, int ping, int pinb, intenstate i){

  analogWrite(pinr, 0);
  analogWrite(ping, 0);
  analogWrite(pinb, 0);

  switch(i){
    case low:
      analogWrite(pinb, 255);
      break;
    case mid:
      analogWrite(pinr, 250);
      analogWrite(ping, 100);
      break;
    case high:
      analogWrite(pinr, 255);
      break;
    case null:
      analogWrite(pinr, 100);
      analogWrite(ping, 100);
      analogWrite(pinb, 100);
      break;
    case HOLYSHIT:
      analogWrite(pinr, 256);
      analogWrite(ping, 100);
      analogWrite(pinb, 100);
      break;
    default:
      analogWrite(pinr, 100);
      analogWrite(ping, 100);
      analogWrite(pinb, 100);
      break;
  }
}

// notele si duratele si asa generate cu LLM pe baza unui midi si spectrograma wav, full disclosure, gemini 3.1 pro, melodie originala: "unnamed manea".

#define NOTE_C5  523
#define NOTE_EB5 622 // D#5
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_AB5 831 // G#5

const int ceasuMelody[] = {
  // Repeat 1
  NOTE_G5, NOTE_F5, NOTE_AB5, NOTE_F5, NOTE_AB5, NOTE_C5, NOTE_EB5,// NOTE_G5, NOTE_F5,
  
};

const int ceasuNoteDurations[] = {
  // Repeat 1 (16 = short block, 8 = medium block, 4 = long block)
  16, 4, 8, 16, 4, 8, 8, //16, 4,
};

// Automatically calculate the correct number of notes (40 notes total)
const int ceasuNoteCount = sizeof(ceasuMelody) / sizeof(ceasuMelody[0]);



void playMelody_deprecated(int buzzerPin, const int * melody, const int * noteDurations, const int noteCount, int times) {
  //pinMode(buzzerPin, OUTPUT);
  for (int i = 0; i < noteCount; i++) {
    int noteDuration = 1000 / noteDurations[i] * 2;
    // A frequency of 0 represents a rest.
    if (melody[i] > 0) {
      tone(buzzerPin, melody[i], noteDuration);
    }
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }

  if(times > 0)
    playMelody_deprecated(buzzerPin, melody, noteDurations, noteCount, times-1);
}




bool isSinging = false;
int currNoteIndex = 0;
int repeatsLeft = 0;
unsigned long noteStartMilis = 0;
int currWaitTime = 0;
bool isTonePhase = false;

const int * activeMelody;
const int * activeDurations;
int activeNoteCount = 0;
int activeBuzzerPin = 0;

void triggerMelody(int buzzerPin, const int * melody, const int * noteDurations, const int noteCount, int times){
  if(isSinging) return;
  
  isSinging = true;
  repeatsLeft = times;
  currNoteIndex = 0;
  
  activeMelody = melody;
  activeDurations = noteDurations;
  activeNoteCount = noteCount;
  activeBuzzerPin = buzzerPin;
  
  int baseDuration = (1000 / activeDurations[0]) * 2;
  if(activeMelody[0] > 0){
    tone(activeBuzzerPin, activeMelody[0], baseDuration);
  }
  
  noteStartMilis = millis();
  currWaitTime = baseDuration;
  isTonePhase = true;
}

void updateMelody(){
  if(!isSinging) return;

  if(millis() - noteStartMilis >= currWaitTime){
    int baseDuration = (1000 / activeDurations[currNoteIndex]) * 2;
    
    if(isTonePhase){
      noTone(activeBuzzerPin);
      currWaitTime = baseDuration * 0.30; 
      isTonePhase = false;
    }else{
      currNoteIndex++;
      
      if(currNoteIndex >= activeNoteCount){
        currNoteIndex = 0;
        repeatsLeft--;
        
        if(repeatsLeft <= 0){
          isSinging = false;
          return;
        }
      }
      
      baseDuration = (1000 / activeDurations[currNoteIndex]) * 2;
      if(activeMelody[currNoteIndex] > 0){
        tone(activeBuzzerPin, activeMelody[currNoteIndex], baseDuration);
      }
      
      currWaitTime = baseDuration;
      isTonePhase = true;
    }
    
    noteStartMilis = millis();
  }
}

int intenstateToInt(intenstate i){
  switch(i){
    case low:
      return 20;
    case mid:
      return 80;
    case high:
      return 255;
    default:
      return 0;
  }

  return 0;
}


void loop() {

  updateMelody();



	
  int curr_milis = millis();
  if (curr_milis - 60 >= milis_for_fade_out){


    intenstate curr = checkIntensity(PMETER);

    Serial.println(intenstateToStr(ist));
    Serial.println(analogRead(PMETER));
    Serial.println(intenstateToStr(curr));
    Serial.println("...");

    //
    setLedColourToInten(ALED_R, ALED_G, ALED_B, curr);



    if(curr != ist){
      //diff logic

      if(curr == high){
        //tone(PASS_BUZZ, 2000, 300);
        Serial.println("SING!");
        //playMelody(PASS_BUZZ, ceasuMelody, ceasuNoteDurations, ceasuNoteCount, 2);
        triggerMelody(PASS_BUZZ, ceasuMelody, ceasuNoteDurations, ceasuNoteCount, 2);
      }

      ist = curr;


      analogWrite(EXTERNAL_LED, intenstateToInt(ist));
      //analogWrite(EXTERNAL_LED, (4096-analogRead(PMETER))/4);

    }

    

    milis_for_fade_out = curr_milis;
    led_out_value /= 1.5;
    if (led_out_value <= 0 || led_out_value >= 255){
      led_out_value = 255;
      
    }
    analogWrite(LED_BUILTIN, led_out_value);
  }
}
