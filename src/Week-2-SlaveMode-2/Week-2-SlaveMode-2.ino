#include <Wire.h>

const int Fan = 8;
const int LED_Motor = 9;
const int Buzzer_Out = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  pinMode(Fan,OUTPUT);
  pinMode(LED_Motor,OUTPUT);
  pinMode(Buzzer_Out,OUTPUT);
}

String receive_str, data_str, sig;
int receive = 0;

void System_FanState(String sig){
  if(sig == "Y"){
    digitalWrite(Fan,HIGH);
    digitalWrite(LED_Motor,HIGH);
    digitalWrite(Buzzer_Out,HIGH);
  }
  else if(sig == "N"){
    digitalWrite(Fan,LOW);
    digitalWrite(LED_Motor,LOW);
    digitalWrite(Buzzer_Out,LOW);
  }
}

void receiveEvent(){
  receive_str = "";
  while(Wire.available()){
    char c = Wire.read();
    if(c == ','){
      break;
    }
    else receive_str += c;
  }
  sig = receive_str.substring(0,1);
  data_str = receive_str.substring(1,receive_str.length());
  Serial.print(data_str);
  Serial.println("|N/A|N/A|");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){
    receive = Serial.read();
    receive = receive - 48;
  }
  if(sig == "Y" && receive == 0){
    sig = "Y";
    System_FanState(sig);
  }
  if(sig == "N" && receive == 0){
    sig = "N";
    System_FanState(sig);
  }
  if(sig == "Y" && receive == 3){
    sig = "N";
    receive = 0;
    System_FanState(sig);
  }
  if(sig == "N" && receive == 3){
    sig = "Y";
    receive = 0;
    System_FanState(sig);
  }
}
