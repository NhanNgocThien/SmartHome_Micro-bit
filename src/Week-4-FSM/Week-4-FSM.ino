#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#include "RTClib.h"
RTC_DS1307 rtc;
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


const int ButtonRead = 2;
const int Buzzer = 13;
const int CDSIn = 1;
const int LED = 0;
const int LED_WARNING = 8;

// Set up range for button 1
const int Lower_1 = 1000;
// Set up range for button 2
const int Upper_2 = 800;
const int Lower_2 = 600;
// Set up range for button 3
const int Upper_3 = 550;
const int Lower_3 = 450;
// Set up Wait state
const int Wait = 400;

int Level_Upper = 500;

unsigned long PrevTime = 0;
unsigned long DelayTime = 0;
bool isPress = false;
bool lastState = false;
int State, OptionCase;
int LightSensitivity = 0;

int fhour, fminute, fsecond, digitChange;
unsigned long HoldTimer = 0;
bool last_State = false;
bool AlarmOn = false;
bool LightMode = false;
bool DarkMode = false;
bool led_state;

void setup(){
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  pinMode(ButtonRead,INPUT);
  pinMode(LED,OUTPUT);
  pinMode(LED_WARNING, OUTPUT);
  pinMode(CDSIn,INPUT);
  pinMode(Buzzer,OUTPUT);
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  State = 1;
}
void System_Clock(){
  DateTime now = rtc.now();
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(daysOfTheWeek[now.dayOfTheWeek()]);
  display.print(" ");
  display.print(now.day());
  display.print("-");
  display.print(now.month());
  display.print("-");
  display.print(now.year());
  display.setCursor(0,10);
  display.print("Time: ");
  display.print(now.hour());
  display.print(":");
  display.print(now.minute());
  display.print(":");
  display.print(now.second());
}

void System_SetClock(){
  fhour = fminute = fsecond = 0;
  digitChange = 1;
  State = 4; 
  display.clearDisplay();
  display.setCursor(0,10);
  display.print("Set Alarm Clock:");
  display.display();
  display.setCursor(50,20);
  display.print(fhour);
  display.print(":");
  display.print(fminute);
  display.print(":");
  display.print(fsecond);
  display.display();
}

void System_PrintMenu(String line1, String line2, String line3, int Cursor){
  display.clearDisplay();
  display.setCursor(0,10*Cursor);
  display.print("> ");
  display.setCursor(10,0);
  display.print(line1);
  display.setCursor(10,10);
  display.print(line2);
  display.setCursor(10,20);
  display.print(line3);
  display.display();
}

void System_SetMenu(int Case){
  String selection_1 = "Set Alarm Clock";
  String selection_2 = "Set Light Sensitive";
  String selection_3 = "Exit Menu";
  switch(Case){
    case 0:{
      System_PrintMenu(selection_1, selection_2, selection_3, 0);
      break;
    }
    case 1:{
      System_PrintMenu(selection_1, selection_2, selection_3, 1);
      break;
    }
    case 2:{
      System_PrintMenu(selection_1, selection_2, selection_3, 2);
      break;
    }
  }
}

void loop(){
  int LightVal = analogRead(CDSIn);\
  Serial.println(LightVal);
  int ButtonState = analogRead(ButtonRead);
  if(millis() - PrevTime >= 1000 && State != 5 && State != 4 && State != 6 && State != 7){
    PrevTime = millis();
    System_Clock();
    if(AlarmOn){
      display.setCursor(0,20);
      display.print("Alarm Set: ");
      display.print(fhour);
      display.print(":");
      display.print(fminute);
      display.print(":");
      display.print(fsecond);
      DateTime now = rtc.now();
      if(now.hour() == fhour && now.minute() == fminute && now.second() == fsecond){
        Serial.println(1);
        digitalWrite(Buzzer,HIGH);
        digitalWrite(LED_WARNING,HIGH);
        State = (LightVal >= Level_Upper)? 3 : 1;
      }
      else Serial.println(0);
    }
    else{
      display.setCursor(0,20);
      display.print("Alarm Turn Off");
    }
    display.display();
  }
  if(millis() - DelayTime >= 50){
   DelayTime = millis();
   switch(State){
    // Light Mode
    case 1:
      if(ButtonState >= Lower_1 && !isPress){
         isPress = true;
         lastState = !lastState;
         digitalWrite(LED,lastState);
      }
      if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
         isPress = true;
         LightMode = true;
         DarkMode = false;
         State = 6;
         OptionCase = 0;
         System_SetMenu(OptionCase);
         break;
      }
      else State = (LightVal >= Level_Upper)? 3 : 1;
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && AlarmOn && !isPress){
        isPress = true;
        AlarmOn = false;
        digitalWrite(Buzzer,LOW);
        digitalWrite(LED_WARNING, LOW);
      }
      if(State == 3){
        lastState = true;
        digitalWrite(LED,lastState);
      }
      break;

   // Dark Mode
   case 3:
      Serial.println(3);
      if(ButtonState >= Lower_1 && !isPress){
         isPress = true;
         lastState = !lastState;
         digitalWrite(LED,lastState);
      }
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && AlarmOn && !isPress){
        isPress = true;
        AlarmOn = false;
        digitalWrite(Buzzer,LOW);
        digitalWrite(LED_WARNING,LOW);
      }
      if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
         isPress = true;
         DarkMode = true;
         LightMode = false;
         State = 6;
         OptionCase = 0;
         System_SetMenu(OptionCase);
         break;
      }
      else State = (LightVal < Level_Upper && !isPress)? 1 : 3;
      if(State == 1){
        lastState = false;
        digitalWrite(LED,lastState);
      }
      break;
      
   // Set Mode On
   case 6:
      if(ButtonState >= Lower_1 && !isPress){
        isPress = true;
        OptionCase++;
        if(OptionCase == 3) OptionCase = 0;
        System_SetMenu(OptionCase);
      }
      if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
        isPress = true;
        if(OptionCase == 0){
          System_SetClock();
        }
        else if(OptionCase == 1){
          State = 7;
          display.clearDisplay();
          display.setCursor(0,10);
          display.print("Set Light-Sensitivity: ");
          display.print(LightSensitivity);
          display.display();
        }
        else if(OptionCase == 2){
          State = (LightMode == true) ? 1 : 3; 
        }
      }
      break;  
      
   // Set Light Sensitivity
   case 7:
      if(ButtonState >= Lower_1 && !isPress){
        HoldTimer = millis();
        isPress = true;
        LightSensitivity++;
        if(LightSensitivity == 3) LightSensitivity = 0;
      }
       if(ButtonState >= Lower_1 && isPress && millis() - HoldTimer >= 2000){
        isPress = true;
        LightSensitivity++;
        if(LightSensitivity == 3) LightSensitivity = 0;
      }
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && !isPress){
        isPress = true;
        switch(LightSensitivity){
          case 0: Level_Upper = 500; break;
          case 1: Level_Upper = 300; break;
          case 2: Level_Upper = 100; break;
        }
        State = 6;
        System_SetMenu(OptionCase);
        break;
      }
      display.clearDisplay();
      display.setCursor(0,10);
      display.print("Set Light Sensitivity: ");
      display.print(LightSensitivity);
      display.display();
      break;

   // Set Alarm Mode
   case 4:
      if(ButtonState >= Lower_1 && !isPress){
        HoldTimer = millis();
        isPress = true;
        State = 5;
      }
       if(ButtonState >= Lower_1 && isPress && millis() - HoldTimer >= 2000){
        isPress = true;
        State = 5;
      }
      if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
        isPress = true;
        digitChange++;
        if(digitChange == 4) digitChange = 1;
      }
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && !isPress){
        isPress = true;
        State = 6;
        AlarmOn = true;
        System_SetMenu(OptionCase);
      }
      break;
      
   case 5:
      switch(digitChange){
        case 1:
          fhour += 1;
          if(fhour == 24) fhour = 0;
          break;
        case 2:
          fminute += 5;
          if(fminute == 60) fminute = 0;
          break;
        case 3:
          fsecond += 10;
          if(fsecond == 60) fsecond = 0;
          break;  
      }
      display.clearDisplay();
      display.setCursor(0,10);
      display.print("Set Alarm Clock:");
      display.display();
      display.setCursor(50,20);
      display.print(fhour);
      display.print(":");
      display.print(fminute);
      display.print(":");
      display.print(fsecond);
      display.display();
      State = 4;
      break;
  }
  if(State != 1 && State != 3 && !isPress){
    if(LightVal >= Level_Upper)
      digitalWrite(LED,HIGH);
    else if(LightVal < Level_Upper) digitalWrite(LED,LOW);
  }
  if(ButtonState <= Wait && isPress) isPress = false;
 }
}
