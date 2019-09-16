#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#include <BH1750FVI.h> // LightLevel Sensor
BH1750FVI LightMeter;

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHT_PIN 0
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

#define YL69In 2 // SolidHumid Sensor

char str_Send[50];
float t, h, Percent, Lux, Lux_Send;
String sig_1, sig_2;
float SolidHumid;

const int SolidMin = 60;
const int SolidMax = 80; 

const int LUXMIN = 990;
const int LUXMAX = 1000;

unsigned long Transmit = 0;
unsigned long PrevTime = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Debug Purpose
  Wire.begin();
  LightMeter.begin();
  LightMeter.SetAddress(0x23);
  LightMeter.SetMode(Continuous_H_resolution_Mode);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();

  dht.begin();
  pinMode(YL69In,INPUT);
}

String System_CheckLightLevel(int Lux){
  if(Lux <= LUXMIN){
    return "Y";
  }
  else if(Lux >= LUXMAX)
    return "N";
}

String System_CheckSolidCondition(int percent){
  if(percent > SolidMin)
    return "N";
  else if(percent <= SolidMin)
    return "Y";
 }

void loop(){
  // Read value from sensor
  t = dht.readTemperature();
  h = dht.readHumidity();
  SolidHumid = 1023 - analogRead(YL69In);
  Percent = map(SolidHumid, 0, 1023, 0, 100);
  sig_1 = System_CheckSolidCondition(Percent);
  Lux = LightMeter.GetLightIntensity();
  sig_2 = System_CheckLightLevel(Lux);
  if(Lux >= 1000) Lux_Send = 1000;
  else Lux_Send = Lux;
  
  // Display and transmit
  if(millis() - Transmit >= 2000){
    Transmit = millis();
    //Display to screen
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Air: ");
    if(isnan(t)) display.print("N/A");
    else display.print((int)t);
    display.print(" C & ");
    if(isnan(h)) display.print("N/A");
    else display.print((int)h);
    display.print("%");
    
    display.setCursor(0,10);
    display.print("Solid Humid: ");
    if(isnan(Percent)) display.print("N/A");
    else display.print((int)Percent);
    
    display.setCursor(0,20);
    display.print("Light Lvl: ");
    if(isnan(Lux)) display.print("N/A");
    else display.print((int)Lux);
    display.print(" LX");
    display.display();
 
     // Send Data to Arduino
    Wire.beginTransmission(8);
    if(isnan(t) || isnan(h)){
      String lengthCheck = "";
      lengthCheck += sig_1 + sig_2;
      lengthCheck += (String)("N/A") + "|" + (String)("N/A") + "!";
      lengthCheck += (String)((int)Percent) + "?" + (String)((int)Lux_Send);
      lengthCheck.toCharArray(str_Send,50);
      Serial.println(str_Send);
      Wire.write(str_Send);
      if((lengthCheck.length() - 1) < 30)
        Wire.write(','); 
    }
    else if(isnan(Percent)){
      String lengthCheck = "";
      lengthCheck += "N" + sig_2;
      lengthCheck += (String)((int)t) + "|" + (String)((int)h) + "!";
      lengthCheck += String("N/A") + "?" + (String)((int)Lux_Send);
      lengthCheck.toCharArray(str_Send,50);
      Serial.println(str_Send);
      Wire.write(str_Send);
      if((lengthCheck.length() - 1) < 30)
        Wire.write(','); 
    }
    else if(isnan(Lux)){
      String lengthCheck = "";
      lengthCheck += sig_1 + "N";
      lengthCheck += (String)((int)t) + "|" + (String)((int)h) + "!";
      lengthCheck += (String)(int(Percent)) + "?" + (String)("N/A");
      lengthCheck.toCharArray(str_Send,50);
      Serial.println(str_Send);
      Wire.write(str_Send);
      if((lengthCheck.length() - 1) < 30)
        Wire.write(','); 
      Wire.endTransmission();
    }
    else{
      String lengthCheck = "";
      lengthCheck += sig_1 + sig_2;
      lengthCheck += (String)((int)t) + "|" + (String)((int)h) + "!";
      lengthCheck += (String)((int)Percent) + "?" + (String)((int)Lux_Send);
      lengthCheck.toCharArray(str_Send,50);
      Serial.println(str_Send);
      Wire.write(str_Send);
      if((lengthCheck.length() - 1) < 30)
        Wire.write(',');
    }
    Wire.endTransmission();
  }
}
