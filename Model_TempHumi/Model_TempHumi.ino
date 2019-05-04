
#include <DHT.h>
#include <ESP8266WiFi.h>

#include <PubSubClient.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

const char* mqtt_server = "119.205.235.214"; //브로커 주소

//const int sleepSeconds = 300; //to save power i send the ESP to sleep

const char* outTopic = "ModelTempHumi/result"; // 밖으로 내보내는 토픽.
const char* clientName = "700303Client";  // 다음 이름이 중복되지 않게 꼭 수정 바람 - 생년월일 추천
const char* setWifiManagerName = "Auto_Model_Temp_Humi_Connect_AP";

WiFiClient espClient;
PubSubClient client(espClient);

float t,h;
char humi[10];
char temp[10];
char msg[128];

String url="";

//#define DHTPIN D2
#define DHTPIN 5  //==wemos d1 -> pin D3   (pin은  esp-8266 pin 기중으로 설정하고  꼽을땐 대응되는 핀에 꼽아야 한다.)
#define DHTTYPE DHT21
 
DHT dht(DHTPIN, DHTTYPE);
//WiFiClient client;

 String inString;
 String topis;

 long interval = 30000; 
 long lastMsg = 0;
 
void setup() 
{
  Serial.begin(115200);
 
 dht.begin();   
 
  pinMode(D0, WAKEUP_PULLUP);

  WiFiManager wifiManager;
    
  wifiManager.autoConnect(setWifiManagerName);
  delay(50);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //wifiManager.startConfigPortal("OnDemandAP");
} 

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
 
}

// mqtt 통신에 지속적으로 접속한다.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientName)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Reconnected");
      // ... and resubscribe    
      
     client.subscribe("ModelTempHumi/PleaseTempHumi");    
        
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

void sendTemperature(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  //delay(3000);
  h = dht.readHumidity(); 
 
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
   
delay(3000);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }else{
    // Convert to String the values to be sent with mqtt
    dtostrf(h,4,2,humi);
    dtostrf(t,4,2,temp);    
  }
    //다시 클라이언트로 현재의 버튼 상황을 보낸다. & mqtt서버에 마지막 정보를 또한 저장된다. 항상클라이언트는 마지막 정보를 받는다.
      sprintf(msg,"|Temp=%s|Humi=%s|",temp,humi);  
      
    Serial.println(msg);   
     
     //true옵션 -> retained 옵션 설정시 마지막 메시지가 broker에 큐 형태로 있다가
     //다른 subcribe가 접속하면 큐에있던 메시지를 보낸다.-> 마지막 상태를 알수 있다.    
    client.publish(outTopic,msg,true); 

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

}


