/*
 * OTA 와 wifiManager 결합한
 * 기본이 되는 코드 입니다.
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>



void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  const char *hostname = "Auto_host";
  WiFiManager wifiManager;  

   if (!wifiManager.autoConnect(hostname)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

  ArduinoOTA.onStart([]() {
    Serial.println("Start OTA");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("End OTA");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //test
  // pinMode(LED_BUILTIN, OUTPUT);
  //여기에 코드 입력.

  
}

void loop() {

  // 여기에 코드 입력
    //test
/*
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  
*/
  
  ArduinoOTA.handle();  
}
