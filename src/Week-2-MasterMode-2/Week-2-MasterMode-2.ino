#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <MQ135.h>

#define PIN_MQ135 1
MQ135 MQ135_Sensor = MQ135(PIN_MQ135);

//sensor a coefficient value
float a = 116.6020682;
//sensor b coefficient value
float b = -2.769034857;

#define DHT_PIN 0
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

char str_Send[50];
String sig;
float t, h, CO2ppm;
float TempMax = 20;
float TempMin = 10;

float C02LevelMax = 300;
float C02LevelMin = 200;

unsigned long Transmit = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Debug Purpose
  Wire.begin();
 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();

  dht.begin();
}

float System_ReadC02Level(int t, int h){
  float R0 = MQ135_Sensor.getCorrectedRZero(t,h);
  float Rs = MQ135_Sensor.getCorrectedResistance(t,h);
  float ppm = a * pow((float)Rs / (float)R0, b);
  return ppm;
}

void loop(){
  // Read value from sensor
  t = dht.readTemperature();
  h = dht.readHumidity();
  if(isnan(t)) Serial.println("N/A"); else Serial.println(t);
  CO2ppm = System_ReadC02Level(t,h);
  sig = System_CheckCondition(t,h);
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
    display.print("C02 Level: ");
    if(isnan(CO2ppm)) display.print("N/A");
    else display.print(CO2ppm);
    display.print(" PPM");
    display.setCursor(0,20);
    if(sig == "Y")
      display.print("Alert: On");
    else
      display.print("Alert: Off");
    display.display();
 
     // Send Data to Arduino
    Wire.beginTransmission(8);
    if(isnan(t) || isnan(h)){
      String lengthCheck = "";
      lengthCheck += "N";
      lengthCheck += (String)("N/A") + "|" + (String)("N/A") + "|";
      lengthCheck += (String)CO2ppm ;
      lengthCheck.toCharArray(str_Send,50);
      Serial.println(str_Send);
      Wire.write(str_Send);
      if((lengthCheck.length() - 1) < 30)
        Wire.write(','); 
    }
    else if(isnan(CO2ppm)){
     String lengthCheck = "";
      lengthCheck += sig;
      lengthCheck += (String)((int)t) + "|" + (String)((int)h) + "|";
      lengthCheck += (String)("N/A"); ;
      lengthCheck.toCharArray(str_Send,50);
      Serial.println(str_Send);
      Wire.write(str_Send);
      if((lengthCheck.length() - 1) < 30)
        Wire.write(','); 
    }
    else{
      String lengthCheck = "";
      lengthCheck += sig;
      lengthCheck += (String)((int)t) + "|" + (String)((int)h) + "|";
      lengthCheck += (String)CO2ppm ;
      lengthCheck.toCharArray(str_Send,50);
      Serial.println(str_Send);
      Wire.write(str_Send);
      if((lengthCheck.length() - 1) < 30)
        Wire.write(','); 
    }
    Wire.endTransmission();
  }
}

String System_CheckCondition(int t, float C02ppm){
  if(t >= TempMax || C02ppm >= C02LevelMax){
    return "Y";
  }
  else if(t <= TempMin || C02ppm <= C02LevelMin)
    return "N";
}
