
#include <Arduino.h>

const int SR_SHCP = 18;
const int SR_STCP = 22;
const int SR_DS = 23;
const int DISP_D[4] = {25,33,32,35};
const int disp_count = 4;

typedef uint8_t byte;

void LOAD_SHIFT_REGISTER(byte DATA){
  digitalWrite(SR_STCP, LOW);
  for(byte i = 0; i < 8; ++i){
    digitalWrite(SR_SHCP, LOW);

    digitalWrite(SR_DS, DATA & 0b1);
    DATA >>= 1;
    digitalWrite(SR_SHCP, HIGH);


    digitalWrite(SR_SHCP, LOW);
  }
  digitalWrite(SR_STCP, HIGH);
}

void setup(){
  pinMode(SR_DS, OUTPUT);
  pinMode(SR_SHCP, OUTPUT);
  pinMode(SR_STCP, OUTPUT);
  digitalWrite(SR_DS, LOW);
  digitalWrite(SR_SHCP, LOW);
  digitalWrite(SR_STCP, LOW);

  for(byte i = 0; i < disp_count; ++i){
    pinMode(DISP_D[i], OUTPUT);
    digitalWrite(DISP_D[i], HIGH);
  }


  digitalWrite(DISP_D[0], LOW);

  LOAD_SHIFT_REGISTER(0b11110110);
}

int seg = 0;

byte to_show = 0;

void loop(){
  digitalWrite(DISP_D[seg], HIGH);
  ++seg;
  //LOAD_SHIFT_REGISTER(to_show++);
  if(seg >= 4) seg = 0;
  digitalWrite(DISP_D[seg], LOW);
  delay(300);
}