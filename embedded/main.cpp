#include <Arduino.h>
#include <WiFi.h>
#include <env.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define ledPin 5   //GPIO pin
const int trigPin = 5;
const int echoPin = 18;

float period;
int dist;
int analog;
int waterLevel = 0; 
String Address;

void setup() {
  
  
  pinMode(trigPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);
  WiFi.begin(home, pw);
  Serial.print("Attempting to connect to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
	Serial.println("");
	Serial.print("Connected to WiFi: network IP Address: ");
	Serial.println(WiFi.localIP());

}

void loop() {
  
  HTTPClient http;
  http.begin("192.168.216.5:3000/tank");
  
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> tank;
  tank["tank_id"]= WiFi.macAddress();
  tank["water_level"]= get_waterLevel();
  String httpRequestData = "";
  serializeJson(tank, httpRequestData);
  //String httpRequestData = "{\"tank_id\": \"Address\", \"water_level\": waterLevel}";
  int httpResponseCode = http.POST(httpRequestData);  

  if(httpResponseCode>0){
    String payload = http.getString();
    Serial.println(payload);
    String led = payload.substring(46,50);
    Serial.println(led);
    Serial.println(httpResponseCode);
    if(led == "true")   //water level between 80-100
    {
        digitalWrite(ledPin,HIGH);
    }
    else
    {
        digitalWrite(ledPin,LOW);
    }
  }
  else
  {

    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);

  }

  http.end();
  delay(2000);
}

int get_waterLevel(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
 
  period = pulseIn(echoPin, HIGH);
  dist = microsendsToCentimeters(period);
  analog = map(dist, 0, 1023, 0, 255);
  Serial.print(dist);
  return dist;

}

float microsendsToCentimeters(float duration)
{
  return duration*0.034/2;
}
