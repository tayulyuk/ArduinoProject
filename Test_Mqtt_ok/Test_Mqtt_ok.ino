/* HDC1000.h  A library for HDC1000 1.0
*/
#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
#include <stdlib.h>
#include <Wire.h>

char temperatureString[5];
char humidityString[5];
IPAddress server(14,63,171,33);  //your mqtt server adress
char ssid[] = "KT_GiGA_2G_76C7";           // your network SSID (name)
char pass[] = "4jf38gf684";           // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status


//HDC1000 mySensor(0x41, 2) <-- DRDYn enabled and connected to Arduino pin 2 (allows for faster measurements).

WiFiEspClient esp8266Client;
PubSubClient client(esp8266Client);
SoftwareSerial esp8266(3, 2); // RX, TX to ESP-01  (10, 11)

void setup(){
  Serial.begin(9600);
  esp8266.begin(9600);  //software serial to ESP8266
  WiFi.init(&esp8266); //ESP8266 wifi

  // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
    // don't continue
    while (true);
    }
  // attempt to connect to WiFi network
    while ( status != WL_CONNECTED) {
       Serial.print("Attempting to connect to WPA SSID: ");
       Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    }
  // you're connected now, so print out the data
    Serial.println("You're connected to the network");
  //connect to MQTT server
    client.setServer(server,1883);
    //client.setCallback(callback);
}

void loop(){
  int temperature = 0;
  int humidity =11;
  dtostrf(temperature,3,0,temperatureString);
  dtostrf(humidity,3,0,humidityString);
  Serial.print("Temperature: ");
  Serial.print(temperature); 
  Serial.print("C, Humidity: ");     
  Serial.print(humidity);
  Serial.println("%");
  
  // mqtt out
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    client.connect("arduinoClient1"); 
    Serial.println("connected");
    delay (1000);
    client.publish("temperature",temperatureString);
    delay (1000);
    client.publish("humidity",humidityString);
    delay (1000);
     }  
  client.publish("temperature",temperatureString);
  delay (1000);
  client.publish("humidity",humidityString);
  delay (1000);
  client.loop();
  delay(3000);
}


