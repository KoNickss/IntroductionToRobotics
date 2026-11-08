
#include <Arduino.h>
#include <sys/types.h>

const int INTERNAL_LED = 2;
const int SR_SHCP = 18;
const int SR_STCP = 22;
const int SR_DS = 23;
const int DISP_D[4] = {25,33,32,26};
const int JOY_CLICK = 19;
const int JOY_X = 35;
const int JOY_Y = 34;
const int disp_count = 4;

typedef uint8_t byte; // E LEGAL! NU FOLOSESC ARDUINO BYTE!!!


typedef enum {COUNTING, STOPPED, SHOW_TARGET, SHOW_HIGHSCORE} GAMESTATE;

int highscore = 9999;

GAMESTATE gamestate = SHOW_TARGET;
int target = 0;

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

byte SCREEN_BUFFER[4] = {0,0,0,0};

void emptyScreenBuffer(){
  SCREEN_BUFFER[0] = 0;
  SCREEN_BUFFER[1] = 0;
  SCREEN_BUFFER[2] = 0;
  SCREEN_BUFFER[3] = 0;
}

const int digits[10] = {
0b11111100, // 0
0b01100000, // 1
0b11011010, // 2
0b11110010, // 3
0b01100110, // 4
0b10110110, // 5
0b10111110, // 6
0b11100000, // 7
0b11111110, // 8
0b11110110, // 9
};

void loadToScreen(int nr){

  emptyScreenBuffer();
  int _nr = nr;

  int sb = 3;

  if(_nr == 0){
    SCREEN_BUFFER[3] = digits[0];
    return;
  }

  while(_nr > 0){
    SCREEN_BUFFER[sb] = digits[_nr % 10];
    _nr /= 10;
    --sb;
    if(sb < 0){
      return;
    }
  }
}

int NR = 0;

void setup(){
  Serial.begin(115200);
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

  //LOAD_SHIFT_REGISTER(0b11110110);


  pinMode(JOY_CLICK, INPUT); // am folosit pull-up fizic ca sa imi fac obiceiuri bune, nu toate MC-urile au internall pull-ups
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);

  pinMode(0, INPUT); // lasat in aer, antena
  target = random(10000);
  NR = target;
  loadToScreen(NR);
  randomSeed(analogRead(0));
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

volatile bool buttonPressNeedsHandling = 0;

int clicks = 0;

void trackBP(){
  static u_int64_t time_stamp = 0;
  static volatile bool check = 0;
  static volatile bool state = LOW;
  u_int64_t curr_milis = millis();
  bool val = digitalRead(JOY_CLICK);

  if(check){
    if(curr_milis - 70 >= time_stamp){
      if(val == LOW && state == LOW){
        time_stamp = 0;
        Serial.println("CLICK!");
        Serial.println(clicks);
        ++clicks;
        
        buttonPressNeedsHandling = 1;
        
      }
      check = 0;
    }
  }else{
    if(val == LOW && state == HIGH){
      check = 1;
      time_stamp = curr_milis;
    }
  }

  state = val;
}


void workScreen(){
  static int time_stamp = 0;
  static int seg = 0;
  int curr_milis = millis();

  if(curr_milis - 5 >= time_stamp){
    time_stamp = curr_milis;
    digitalWrite(DISP_D[seg], HIGH);
    ++seg;
    
    if(seg >= 4) seg = 0;

    LOAD_SHIFT_REGISTER(SCREEN_BUFFER[seg]);
    
    digitalWrite(DISP_D[seg], LOW);
  }
}

void screenLoadLine(){
  SCREEN_BUFFER[0] = 2;
  SCREEN_BUFFER[1] = 2;
  SCREEN_BUFFER[2] = 2;
  SCREEN_BUFFER[3] = 2;

}

void screenAppendLeadingE(){
  SCREEN_BUFFER[0] = 0b10011111;
}


void doGameLogic(){
  int X = analogRead(JOY_X);
  int Y = analogRead(JOY_Y);
  static unsigned long timestamp = 0;
  unsigned long curr_milis = millis();

  if(gamestate != COUNTING && buttonPressNeedsHandling){
    buttonPressNeedsHandling = 0;
  }

  if(gamestate == SHOW_TARGET && X <= 10){
    gamestate = SHOW_HIGHSCORE;
    loadToScreen(highscore);
    screenAppendLeadingE();
    timestamp = curr_milis;
    
  }

  if(gamestate == SHOW_HIGHSCORE){
    if(curr_milis - 3000 >= timestamp){
      loadToScreen(target);
      gamestate = SHOW_TARGET;
    }
  }

  if(gamestate == SHOW_TARGET && Y >= 4090){
    gamestate = COUNTING;
    NR = 9999;
    loadToScreen(NR);
  }else if(gamestate == COUNTING){
    if(buttonPressNeedsHandling){
      buttonPressNeedsHandling = 0;
      gamestate = STOPPED;
      int error = abs(NR - target);
      if(error <= 999){
        loadToScreen(error);
        screenAppendLeadingE();
              Serial.println("E-");
        if(error < highscore){
          highscore = error;
        }

      }else{
        screenLoadLine();
              Serial.println("LINE");

      }
      Serial.println(error);
      
    }
    if(curr_milis - 2 >= timestamp){
      --NR;
      timestamp = curr_milis;
      loadToScreen(NR);
      if(NR <= 0){
        NR = 0;
        gamestate = STOPPED;
        screenLoadLine();
      }
    }
  }else if(gamestate == STOPPED && Y <= 10){
    target = random(10000);
    NR = 9999;
    gamestate = SHOW_TARGET;
    loadToScreen(target);
  }
  
}

void loop(){

  blinkMe(INTERNAL_LED);
  workScreen();
  trackBP();
  doGameLogic();
  
}