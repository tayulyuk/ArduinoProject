#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//위와 아래가 공통되는 첨부.
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


WiFiClient espClient;
PubSubClient client(espClient);

 WiFiManager wifiManager;
 
void setup() {
  
   
  Serial.begin(115200);
  Serial.println("Booting");  
   
//  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  /*
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
*/
    
 // wifiManager.autoConnect("Auto_AP_Test");
  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
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
   pinMode(LED_BUILTIN, OUTPUT);
  
}
String myName;
int age;
void loop() {
  ArduinoOTA.handle();
  Serial.print("ok boot ok");
  delay(3000);
/*
  while (Serial.available()==0) {             //Wait for user input
  
  }
  myName=Serial.readString();                 //Read user input into myName
  
  Serial.println("How old are you? ");        //Prompt User for input
  while (Serial.available()==0)  {
  
  }
  age=Serial.parseInt();   
 Serial.print(" string : ");
 Serial.println(myName);
  Serial.print(" int : ");
 Serial.print(age);
 

   if(Serial.available() ) 
  {
    char c = Serial.read();
    if(c == '\n')
    {
      parseCommand(command);
      command = "";      
    }
    else
    {
      command += c;
    }
    */
  //  delay(3000);
  //  WiFi.disconnect();

  //test

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  


    delay(3000);
    
  //  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
  // Serial.println("Connection Failed! Rebooting... okkkkkk");
  //  delay(5000);
  Serial.print("reset~ ");
    ESP.restart();
 // }
  }
/*
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because 
                                    // it is active low on the ESP-01)
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);         
  */        


void parseCommand(String com)
{
  String part1;
  String part2;  
  part1 = com.substring(0,com.indexOf(" "));
  part2 = com.substring(com.indexOf(" ") + 1);

  
  if(part1.equalsIgnoreCase("pinOn"))
  {   
   Serial.print("on 1 :" + part1);
   Serial.print("on 1 :" + part2);
  } 
  else if (part1.equalsIgnoreCase("pinOff"))
  {   
    Serial.print("off 2 :" + part1);
   Serial.print("off 2 :" + part2);
    }
}


