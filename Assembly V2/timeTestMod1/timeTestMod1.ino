#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"

RTC_PCF8523 rtc;

#define DHTPIN 2                                                                  //dht shindig
#define moistPin A0
#define ledPin 5
#define DHTTYPE DHT22                                                             //schwoooopz
DHT dht(DHTPIN, DHTTYPE);                                                         //who really knows? all this stuff is copied from DHTtester example code.

void moistureCheck();
void temperatureCheck();

float tempdesired = 26.0;                                                         //set desired temperature
int moisturevalue = 570;                                                        //set the desired moisture reading at which the solenoid is switched
bool heatState = false;

void setup() {
  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  dht.begin();                                                                         //initialize the dht sensor

}

void loop() {
  DateTime now = rtc.now();
  int timeHour = now.hour();
  int timeMinute = now.minute();

  if ( (timeHour >= 6) && (timeHour < 18) ) {                                   // Only monitor system between 6am and 6pm
    digitalWrite(ledPin,HIGH);
    
    static unsigned long prevTime = 0;
    unsigned long currTime = (timeHour - 6) * 60 + timeMinute, waitTime = 1;   // Set time to count number of minutes since 6am, set dead time to be 30 minutes
    
    Serial.print("Current time: ");
    Serial.print(currTime);
    Serial.print("   Previous time: ");
    Serial.println(prevTime);
    
    if ( abs(currTime - prevTime) >= waitTime) {
      Serial.println("I did a thing!");
      prevTime = currTime;

      moistureCheck();                                                        //the yeet meat

      temperatureCheck();                                                     //yeet feet
    }
  }
  
  else {
    digitalWrite(ledPin,LOW);
  }
  
  overheatCheck();

  
  delay(5000);

}

void moistureCheck() {
  
  int avgMoist = 0;
  int numReads = 5;
  
  for (int i = 0; i < numReads; i++){
    int moistValue = analogRead(moistPin);
    avgMoist = avgMoist + moistValue;
    delay(1000);
  }

  avgMoist = avgMoist / numReads;
  Serial.print("Moisture value: ");
  Serial.println(avgMoist);
  
  if (avgMoist > moisturevalue) {                                       //if the read is greater than 620, it is too dry
    Serial.println("Moisture too low; adding water");
    digitalWrite(6, HIGH);                                   //in this instance the water is 'turned on' via switchVar = true.
    delay(10000);                                             //to be calibrated
    digitalWrite(6, LOW);
  }
  
  else {
    // digitalWrite(6, LOW);                                     //turned off ... unnecessary code technically. The above 'if' technically covers everything
  }
  
  //Serial.print("Moisture is: ");
  //Serial.println(avgMoist);
}

void temperatureCheck() {
  
  float avgTemp = 0;
  int numReads = 5;
  
  for (int i = 0; i < numReads; i++){
    float t = dht.readTemperature();
    avgTemp = avgTemp + t;
    delay(1000);
  }
  
  avgTemp = avgTemp / float(numReads);
  
  Serial.print("Average temperature reading:");
  Serial.println(avgTemp);
  //float readTempVariable = digitalRead(6);
  
  if (avgTemp < tempdesired) {
    Serial.println("Heat too low; heating on");
    heatState = true;
  }
  digitalWrite (7,heatState);
}

void overheatCheck() {
  
  float t = dht.readTemperature();
  
  if (t > tempdesired){
    heatState = false;
    Serial.print("Heating off. Current temp: ");
    Serial.println(t);

  }
  
  digitalWrite (7,heatState);
}
