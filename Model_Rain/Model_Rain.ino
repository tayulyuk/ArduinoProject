
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//ota
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>


//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

const char* mqtt_server = "119.205.235.214"; //브로커 주소
const char* outTopic = "Hagabi/rain"; // 밖으로 내보내는 토픽.
//ModelTempHumi/result
//1029931969033/temp1
const char* clientName = "HahabiRain";  // 다음 이름이 중복되지 않게 꼭 수정 바람 - 생년월일 추천
//hagabi3class
const char* setWifiManagerName = "Auto_Hagabi_Rain_AP";

WiFiClient espClient;
PubSubClient client(espClient);

char msg[128];

String url="";

 String inString;
 String topis;

 long interval = 30000; 
 long lastMsg = 0;

 int Raindrops_pin = A0;  // 빗방울센서 핀을 A0으로 설정
 
void setup() 
{
  Serial.begin(115200);

  pinMode(A0 , INPUT);   // 빗방울 감지핀을 아날로그 입력으로 설정
  
 dht.begin();   

  WiFiManager wifiManager;
    
 if (!wifiManager.autoConnect(setWifiManagerName)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  //-- OTA setup start
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
  //----- end OTA setup
  
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //test
  // pinMode(LED_BUILTIN, OUTPUT);
} 

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
  //받은 메세지가 없다..
}

// mqtt 통신에 지속적으로 접속한다.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
  //  Serial.print("Attempting MQTT connection...");  
    if (client.connect(clientName)) {
      /*
   //   Serial.println("connected"); //-----------------------------------------
      // Once connected, publish an announcement...
     // client.publish(outTopic, "Reconnected");
      // ... and resubscribe    
      
     //client.subscribe("ModelTempHumi/PleaseTempHumi");    
        */

        // 온도 측정에선 이공간을 비워 둬라.
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds"); //--------------------5초는 너무 길다.
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

void sendRainMessage(){
   
  int rain = analogRead(A0); 
   
  if (isnan(rain)) {
    Serial.println("Failed to read from Rain sensor!");
    return;
  }

  int isRain = 0; // 0 이면 안옴   / 1이면 비옴 ㅋ
  delay(100);                       // 센서값 간격 0.1초로 설정
  if(rain < 500){         // 빗방울 감지핀의 수위가 500미만이면  비 옴
    isRain = 1;
  }                                  
 else {                              // 빗방울 감지핀의 출력값이 500이상이면 비 안옴  
    isRain = 0;
 } 


  if(storeIsRain != isRain) // 저장한 값이 변동이 있다면  메세지 보내자.
  {
      sprintf(msg,"|Temp=%s|Humi=%s|",temp,humi);  
      
      Serial.println(msg); //-------------------------------------------------  
     
     //true옵션 -> retained 옵션 설정시 마지막 메시지가 broker에 큐 형태로 있다가
     //다른 subcribe가 접속하면 큐에있던 메시지를 보낸다.-> 마지막 상태를 알수 있다.    
    client.publish(outTopic,msg,true); 
    }
}

void loop() 
{   
  //-- 추가 와이파이 start
  while ( WiFi.status() != WL_CONNECTED ) 
  {
    if (WiFi.status() == WL_CONNECTED) 
    {
     Serial.println("WiFi Re_connected");
    }      
  }
  //--  추가 와이파이 end  
  
  if (!client.connected()) { // MQTT
    reconnect();
  }
  client.loop();


  unsigned long now = millis();
 
  if(now - lastMsg > interval) {
    // save the last time you blinked the LED 
    lastMsg = now;
    sendTemperature();
  }

  //OTA
  ArduinoOTA.handle();
}
