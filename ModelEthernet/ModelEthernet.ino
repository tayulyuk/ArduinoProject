/*
 * ethernet shild.
 * 
1.
 pubsubclient.h  파일에서
// MQTT_VERSION : Pick the version
#define MQTT_VERSION MQTT_VERSION_3_1
#ifndef MQTT_VERSION
//#define MQTT_VERSION MQTT_VERSION_3_1_1
버전을 이전으로 돌리고 (주석처리 3_1_1)

2.
ethernet  예시중
webClient 를 사용했다.
///
enc28j60 -> web site 접속만 된다. mqtt 실패.
uno -> 동작 확인 mqtt.
mega2560 -> 동작확인 mqtt.
///
 */
 
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient ethernetClient;
PubSubClient client;

String inString;
const char* ethernetClientName = "ModelEthernet";  // 각 현장마다 다르게 한다.
const char* serverIp = "119.205.235.214";


// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
  
  //inString = "";
  Serial.print("Message arrived [");   //---------------------------------------------
  Serial.print(topic);//------------------------------------------------------------------------ test 후 꼭 주석.
  Serial.print("] ");//--------------------------------------------------
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  //-------------------------------- 이것도 출력이네  .. print 부분 빼세요.
  }
  Serial.println();

  // payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에
  
 /*
  for (int i = 0; i < length; i++) {
    inString += (char)payload[i];    
  }
  
  Serial.println("getString :" + inString); //-----------------------------------------------
 */
   client.publish("test/result","ok server",true);
  
}

// mqtt 통신에 지속적으로 접속한다.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ethernetClientName)) 
    {
      Serial.println("connected");//---------------------------------------------      
       client.subscribe("test");
    }
    else 
    {
      Serial.print("failed, rc=");   
      Serial.print(client.state()); 
      Serial.println(" try again in 5 seconds");       
      delay(5000);
      //재 시작.
      startEthernetSetting(); 
    }
  }
}

void startEthernetSetting()
{  
  Serial.println("Initialize Ethernet with DHCP:");  
  if (Ethernet.begin(mac) == 0) 
  {
      Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000); 
  
  client.setServer(serverIp, 1883);
  client.setClient(ethernetClient);
  client.setCallback(callback);
}

void setup() {  
  Serial.begin(9600);   
  
  startEthernetSetting();
}


void loop() { 
  if (!client.connected()) {
    reconnect();
  }  
  client.loop();
}
