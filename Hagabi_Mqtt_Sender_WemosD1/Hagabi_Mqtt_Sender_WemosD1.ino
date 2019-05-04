//해가비 하우스 

//통신으로 받은 값을 메가로 전송만 한다

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//OTA
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

// 아래의 6개설정은 사용자 환경에 맞게 수정하세요.
//const char* ssid = "KT_GiGA_2G_76C7"; // 와이파이 AP, 또는 스마트폰의 핫스판 이름
//const char* password = "4jf38gf684";  // 와이파이 AP, 또는 스마트폰의 핫스판 이름

const char* mqtt_server = "119.205.235.214"; //브로커 주소

// 이 주소는 GUID 활용하자
//1동 -> Hagabi_1_Auto_AP_Setting &  2동 -> Hagabi_2_Auto_AP_Setting
const char* wifi_name ="Hagabi_1_Auto_AP_Setting";

const char* outTopic = "Hagabi/result"; // 밖으로 내보내는 토픽. -> 1,2동 같은 결과로 받는다.
// GUID를 활용하자.
//test1 ->1동  test2 -> 2동
const char* subscribeName = "Hagabi/test1";

const char* clientName = "980303Client";  // 다음 이름이 중복되지 않게 꼭 수정 바람 - 생년월일 추천

WiFiClient espClient;
PubSubClient client(espClient);

String inString="";
String topis ="";

char* mchar; //클라이언트로 부터 버튼의 값을 받는다. 이것으로 각 버튼에게 저장한다
char *messge;// 버튼들의 값을 모아서 클라이언트로 보내기 위한 것.

String command; //mega로 부터 받는 메세지 저장.

void setup() { 
    
  Serial.begin(115200);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around   
  WiFiManager wifiManager;    
  
   if (!wifiManager.autoConnect(wifi_name)) {
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
}


// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  //작업할거
//////// 들어온 값이 이거라면   이렇게 write 써라  
///메가로 신호를 보내고
/// 다시 받은 값을 비교해서 같다면
////다음 작업을 해라 

   //client.publish(outTopic,messge,true); 
   //다른 subcribe가 접속하면 큐에있던 메시지를 보낸다.-> 마지막 상태를 알수 있다.     
   
}

// mqtt 통신에 지속적으로 접속한다.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
   // Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientName)) {
    //  Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(outTopic, "Reconnected");
      // ... and resubscribe    

      // GUID를 활용하자.
     client.subscribe(subscribeName);     
        
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      // Wait 3 seconds before retrying
      delay(1000);
    }
  }
}

void loop() {

 //-- 추가 와이파이 start
  while ( WiFi.status() != WL_CONNECTED ) 
  {
    if (WiFi.status() == WL_CONNECTED) 
    {
     Serial.println("WiFi Re_connected");
    }      
  }
  //--  추가 와이파이 end  

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

//하부 mega로 부터 받는 정보
//이 정보를 다시 클라이언트에 보낸다.
  if(Serial.available() ) 
  {
    char c = Serial.read();
    if(c == '\n')
    {
      SendCommand(command);
      command = "";      
    }
    else
    {
      command += c;
    }
  }

   //OTA
  ArduinoOTA.handle();
}

//클라이언트에 받은 값을 돌려 보낸다.
void SendCommand(String msg)
{
  char* cm = (char*)calloc((msg.length() + 1),sizeof(char)); 
  msg.toCharArray(cm,msg.length()); 
  client.publish(outTopic, cm);    
  free(cm);
}

