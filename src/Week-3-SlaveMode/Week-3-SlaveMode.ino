#include <Wire.h>

const int Pump = 8;
const int LED_Motor = 9;
const int LED_Light = 10;
const float led = 1.023;

void setup(){
  Serial.begin(9600);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  pinMode(Pump,OUTPUT);
  pinMode(LED_Motor, OUTPUT);
  pinMode(LED_Light,OUTPUT);
}

String receive_str, data_str, temp_humid, lux;
int lux_int;
String sig_1, sig_2;
int receive = 0;

void receiveEvent(){
  receive_str = "";
  while(Wire.available()){
    char c = Wire.read();
    if(c == ','){
      break;
    }
    else receive_str += c;
  }
  sig_1 = receive_str.substring(0,1);
  sig_2 = receive_str.substring(1,2);
  int splitT = receive_str.indexOf("!");
  int splitC = receive_str.indexOf("?");
  temp_humid = receive_str.substring(2,splitT);
  Serial.print(temp_humid);
  Serial.print("|N/A|");
  data_str = receive_str.substring(splitT+1, splitC);
  Serial.print(data_str);
  lux = receive_str.substring(splitC+1, receive_str.length());
  Serial.print("|");
  Serial.print(lux);
  Serial.println("|");
  int lux_cal = 1023 - led*(lux.toInt());
  int lux_convert = map(lux_cal,0,1023,0,255);
  lux_int = lux_convert;
}

void System_ChangeLightLevel(String sig){
  if(sig == "Y")
    analogWrite(LED_Light,lux_int);
  if(sig == "N")
    analogWrite(LED_Light,LOW);
}

void System_PumpState(String percent){
  if(percent == "Y"){
    digitalWrite(LED_Motor,HIGH);
    digitalWrite(Pump,HIGH);
  }
  if(percent == "N"){
    digitalWrite(LED_Motor,LOW);
    digitalWrite(Pump,LOW);
  }
}

void loop(){
    if(Serial.available() > 0){
      receive = Serial.read();
      receive = receive - 48;
    }
    // Control Pump
    if(sig_1 == "Y" && receive == 0){
      sig_1 = "Y";
      System_PumpState(sig_1);
    }
    else if(sig_1 == "N" && receive == 0){
      sig_1 = "N";
      System_PumpState(sig_1);
    }
    else if(sig_1 == "Y" && receive == 1){
      sig_1 = "N";
      receive = 0;
      System_PumpState(sig_1);
    }
    else if(sig_1 == "N" && receive == 1){
      sig_1 = "Y";
      receive = 0;
      System_PumpState(sig_1);
    }
  
    // Control Garden Light
     if(sig_2 == "Y" && receive == 0){
      sig_2 = "Y";
      System_ChangeLightLevel(sig_2);
    }
    else if(sig_2 == "N" && receive == 0){
      sig_2 = "N";
      System_ChangeLightLevel(sig_2);
    }
    else if(sig_2 == "Y" && receive == 2){
      sig_2 = "N";
      receive = 0;
      System_ChangeLightLevel(sig_2);
    }
    else if(sig_2 == "N" && receive == 2){
      sig_2 = "Y";
      receive = 0;
      System_ChangeLightLevel(sig_2);
    }
}
