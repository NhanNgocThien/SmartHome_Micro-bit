#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
    
const int LED = 0;
const int LED_WARNING = 8;
const int ButtonRead = 2;
const int PirIn = 1;
const int BuzzerOut = 13;

// Set up range for button 1
const int Lower_1 = 1000;
// Set up range for button 2
const int Upper_2 = 800;
const int Lower_2 = 600;
// Set up range for button 3
const int Upper_3 = 550;
const int Lower_3 = 450;
// Set up wait state
const int Wait = 400;

int ButtonState, SensorVal;
int State = 2;
int Counter1 = 0;
int Counter2 = 0;
int digitChange = 1;
bool isPress = false;
bool ControlSig = false;
bool lastState;
int nextState;

unsigned long SensorDelay = 0;
unsigned long PrevTime = 0;
unsigned long WaitInterVal = 0;
int pass[4] = {0, 0, 0 , 0};
int user[4] = {0, 0, 0, 0};
int repass[4] = {0, 0, 0, 0};
int trials = 0;
int cd1, cd2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(PirIn,INPUT);
  pinMode(ButtonRead,INPUT); 
  pinMode(LED_WARNING,OUTPUT);
  pinMode(LED,OUTPUT); 
  pinMode(BuzzerOut,OUTPUT);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void System_DisplayText(String OLEDPrint){
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(OLEDPrint);
  display.display();  
}

void System_DisplayCount(int count, String OLEDPrint){
  display.setCursor(0,0);
  display.print(OLEDPrint);
  display.print(count);
}

void System_DisplayPass(){
  if(ButtonState >= Lower_1 && !isPress){
     SensorDelay = millis();
     isPress = true;
     State = -2;
  }
  if(ButtonState >= Lower_1 && isPress && millis() - SensorDelay >= 2000){
    isPress = true;
    State = -2;
  }
  if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
     isPress = true;
     digitChange++;
     if(digitChange == 5) digitChange = 1;
  }
  if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && !isPress){
     isPress = true;
     int count = 0;
     for(int i = 0; i < 4; i++){
        if(user[i] != pass[i]){
          trials++;
          break;
        }
        else count++;
     }
     if(count == 4){
        user[0] = user[1] = user[2] = user[3] = 0;
        State = 6;
        digitalWrite(LED,HIGH);
        digitalWrite(LED_WARNING,LOW);
        lastState = true;
        System_DisplayText("Access Granted");
        delay(1000);
     }
     else if(trials == 3){
        digitalWrite(LED_WARNING,HIGH);
        digitalWrite(BuzzerOut,HIGH);
        State = 7;
     }
  }
   display.clearDisplay();
   System_DisplayCount(cd1,"Verify Key: ");
   display.setCursor(0,10);
   display.print("Enter Key: ");
   display.print(user[0]);
   display.print(user[1]);
   display.print(user[2]);
   display.print(user[3]);
   display.setCursor(0,20);
   display.print("Trials: ");
   display.print(3 - trials);
  display.display();
  if(ButtonState <= Wait && isPress) isPress = false;
}

void System_SetCount(int count1){
  display.clearDisplay();
  display.setCursor(25,0);
  display.print("System Set");
  display.setCursor(0,10);
  display.print("CountDown Timer: ");
  display.print(count1);
  display.setCursor(0,20);
  display.print("Set Password: ");
  display.print(pass[0]);
  display.print(pass[1]);
  display.print(pass[2]);
  display.print(pass[3]);
}

void loop() {
  // put your main code here, to run repeatedly:
 ButtonState = analogRead(ButtonRead);
 SensorVal = digitalRead(PirIn);
 Serial.println(ButtonState);
 if(millis() - PrevTime >= 20){
  PrevTime = millis(); 
  switch(State){
    case -2:
      switch(digitChange){
        case 1:
          user[0] += 1;
          if(user[0] == 10) user[0] = 0;
          break;
        case 2:
          user[1] += 1;
          if(user[1] == 10) user[1] = 0;
          break;
        case 3:
          user[2] += 1;
          if(user[2] == 10) user[2] = 0;
          break;
        case 4:
          user[3] += 1;
          if(user[3] == 10) user[3] = 0;
          break;
      }
      State = nextState;
      break;

    case -1:
      switch(digitChange){
          case 1:
            repass[0] += 1;
            if(repass[0] == 10) repass[0] = 0;
            break;
          case 2:
            repass[1] += 1;
            if(repass[1] == 10) repass[1] = 0;
            break;
          case 3:
            repass[2] += 1;
            if(repass[2] == 10) repass[2] = 0;
            break;
          case 4:
            repass[3] += 1;
            if(repass[3] == 10) repass[3] = 0;
            break;
      }
      State = 5;
      break;

     case -3:
      switch(digitChange){
          case 1:
            repass[0] += 1;
            if(repass[0] == 10) repass[0] = 0;
            break;
          case 2:
            repass[1] += 1;
            if(repass[1] == 10) repass[1] = 0;
            break;
          case 3:
            repass[2] += 1;
            if(repass[2] == 10) repass[2] = 0;
            break;
          case 4:
            repass[3] += 1;
            if(repass[3] == 10) repass[3] = 0;
            break;
      }
      State = 8;
      break;
      
    case 0:
     switch(digitChange){
      case 1:
        pass[0] += 1;
        if(pass[0] == 10) pass[0] = 0;
        break;
      case 2:
        pass[1] += 1;
        if(pass[1] == 10) pass[1] = 0;
        break;
      case 3:
        pass[2] += 1;
        if(pass[2] == 10) pass[2] = 0;
        break;
      case 4:
        pass[3] += 1;
        if(pass[3] == 10) pass[3] = 0;
        break;
     }
     State = 3;
     break;
     
    case 1:
      cd1 = Counter1;
      //cd2 = Counter2;
      if(millis() - WaitInterVal >= 500)
        System_DisplayText("System Ready");
      if(SensorVal == 1 && millis() - WaitInterVal >= 1000){
        display.clearDisplay();
        System_DisplayCount(cd1,"Verify Key: ");
        WaitInterVal = millis();
        lastState = false;
        State = 4;
        break;
      }
      break;
     // Set Count Down Timer
    case 2:
      if(ButtonState >= Lower_1 && !isPress){
        SensorDelay = millis();
        Counter1 += 10;
        if(Counter1 == 70) Counter1 = 0;
        isPress = true;
      }
      if(ButtonState >= Lower_1 && isPress && millis() - SensorDelay >= 2000){
        Counter1 += 10;
        if(Counter1 == 70) Counter1 = 0;
        isPress = true;
      }
      System_SetCount(Counter1);
      display.display();
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && !isPress){
        isPress = true;
        digitChange = 1;
        State = 3;
      }
      if(ButtonState <= Wait && isPress) isPress = false;
      break;
      
    case 3:
      System_SetCount(Counter1);
      display.display();
      if(ButtonState >= Lower_1 && !isPress){
        SensorDelay = millis();
        isPress = true;
        State = 0;
      }
      if(ButtonState >= Lower_1 && isPress && millis() - SensorDelay >= 2000){
        isPress = true;
        State = 0;
      }
      if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
        isPress = true;
        digitChange++;
        if(digitChange == 5) digitChange = 1;
      }
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && !isPress){
        isPress = true;
        State = (ControlSig == false)? 1 : 6;
      }
      if(ButtonState <= Wait && isPress) isPress = false;
      break;

    // Security Lock   
    case 4:
      if(millis() - WaitInterVal >= 1000){
        display.clearDisplay();
        System_DisplayCount(cd1,"Verify Key: ");
      }
      if(SensorVal == 1 && millis() - WaitInterVal >= 1000){
        WaitInterVal = millis();
        SensorDelay = millis();
        lastState = !lastState;
        digitalWrite(LED_WARNING,lastState);
        cd1 = cd1 - 1;
      }
      else if(SensorVal == 0 && (millis() - SensorDelay >= 3000)){
        for(int i = 0; i < 4; i++)
        {
          user[i] = 0;
          repass[i] = 0;
        }
        digitChange = 1;
        isPress = true;
        trials = 0;
        State = 1;
        System_DisplayText("System is reseting...");
        digitalWrite(LED_WARNING,LOW);
        digitalWrite(LED,LOW);
        WaitInterVal = millis();
        break;
      }
      if(cd1 == 0){
        State = 7;
        for(int i = 0; i < 4; i++)
        {
          digitChange = 1;
          user[i] = 0;
          repass[i] = 0;
        }
        digitalWrite(LED_WARNING,HIGH);
        digitalWrite(BuzzerOut,HIGH);
      }
      else{
        nextState = 4;
        System_DisplayPass();
        if(State == 6) break;
      }
      display.display();
      break;

    // Reset count down time and password
    case 5:
      display.clearDisplay();
      display.setCursor(0,10);
      display.print("Verify Key: ");
      display.print(repass[0]);
      display.print(repass[1]);
      display.print(repass[2]);
      display.print(repass[3]);
      display.display();
      if(ButtonState >= Lower_1 && !isPress){
        isPress = true;
        State = -1;
        SensorDelay = millis();
      }
      if(ButtonState >= Lower_1 && isPress && millis() - SensorDelay >= 2000){
        isPress = true;
        State = -1;
      }
      if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
        isPress = true;
        digitChange++;
        if(digitChange == 5) digitChange = 1;
      }
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && !isPress){
        isPress = true;
        int count = 0;
        for(int i = 0; i < 4; i++){
          if(repass[i] != pass[i]){
            break;
          }
          else count++;
        }
        if(count == 4) State = 2;
        else{
          display.setCursor(0,20);
          display.print("Invalid Key");
          display.display();
          WaitInterVal = millis();
          State = 6;
        }
      }
      if(ButtonState <= Wait && isPress) isPress = false;
     break;

    // Living Room Mode 
    case 6: 
      if(millis() - WaitInterVal >= 500)
        System_DisplayText("Welcome Home");
      if(ButtonState >= Lower_1 && !isPress){
        lastState = !lastState;
        isPress = true;
        digitalWrite(LED,lastState);
      }
      digitalWrite(LED_WARNING,SensorVal);
      if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
        for(int i = 0; i < 4; i++)
        {
          user[i] = 0;
          repass[i] = 0;
        }
        isPress = true;
        digitChange = 1;
        trials = 0;
        State = 1;
        System_DisplayText("System is reseting...");
        digitalWrite(LED_WARNING,LOW);
        digitalWrite(LED,LOW);
        WaitInterVal = millis();
      }
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && !isPress){
        digitalWrite(LED_WARNING,LOW);
        digitalWrite(LED,LOW);
        repass[0] = repass[1] = repass[2] = repass[3] = 0;
        isPress = true;
        ControlSig = true;
        State = 5;
        digitChange = 1;
      }
      if(ButtonState <= Wait && isPress) isPress = false;
      break;

    // Alert Mode  
    case 7: 
      display.clearDisplay();
      System_DisplayText("Intruder Alert");
      isPress = true;
      State = 8;
      digitChange = 1;
      repass[0] = repass[1] = repass[2] = repass[3] = 0;
      break;

    // Verify pass to turn off buzzer
    case 8:
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Intruder Alert");
      display.setCursor(0,10);
      display.print("Verify Key: ");
      display.print(repass[0]);
      display.print(repass[1]);
      display.print(repass[2]);
      display.print(repass[3]);
      display.display();
      if(ButtonState >= Lower_1 && !isPress){
        isPress = true;
        State = -3;
        SensorDelay = millis();
      }
      if(ButtonState >= Lower_1 && isPress && millis() - SensorDelay >= 2000){
        isPress = true;
        State = -3;
      }
      if(ButtonState >= Lower_2 && ButtonState <= Upper_2 && !isPress){
        isPress = true;
        digitChange++;
        if(digitChange == 5) digitChange = 1;
      }
      if(ButtonState >= Lower_3 && ButtonState <= Upper_3 && !isPress){
        isPress = true;
        int count = 0;
        for(int i = 0; i < 4; i++){
          if(repass[i] != pass[i]){
            break;
          }
          else count++;
        }
        if(count == 4){
           WaitInterVal = millis();
           State = 6;
           digitalWrite(LED, HIGH);
           digitalWrite(BuzzerOut,LOW);
        }
        else{
          display.display();
          WaitInterVal = millis();
          State = 7;
        }
      }
    break;
  }
  if(ButtonState <= Wait && isPress) isPress = false;
 }
}
