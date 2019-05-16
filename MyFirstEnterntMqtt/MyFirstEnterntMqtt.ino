#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// 함수 프로토 타입
void subscribeReceive(char* topic, byte* payload, unsigned int length);
 
// 여기에 MAC 주소와 IP 주소를 설정하십시오.
byte mac [] = {0xDE, 0xA, 0xBE, 0xFF, 0xFE, 0xED};
IPAddress ip (192, 168, 1, 160);
 
// http와 슬래시를 생략하십시오!
const char * server = "sft123.cafe24.com ";
 
// 이더넷과 MQTT 관련 객체
EthernetClient ethClient;
PubSubClient mqttClient (ethClient);

void setup ()
{
  // 디버깅 목적으로 유용합니다.
  Serial.begin (9600);
  
  // 이더넷 연결을 시작합니다.
  Ethernet.begin (mac, ip);              
  
  // 이더넷 부팅에 약간의 시간이 필요합니다!
  delay (3000);                          
 
  // 위에서 설명한 서버로 MQTT 서버를 설정한다. ^
  mqttClient.setServer (server, 1883);   
 
  // "myClientID"ID를 사용하여 서버에 연결하려고 시도합니다.
  if (mqttClient.connect ( "myClientID")) 
  {
    Serial.println ( "연결이 잘 완료되었습니다.");
 
    // 구독 이벤트를 설정합니다.
    mqttClient.setCallback (subscribeReceive);
  } 
  else 
  {
    Serial.println ( "서버 연결이 실패한 것 같습니다 ...");
  }
}

void loop ()
{
  // 이것은 루프의 맨 위에 필요합니다!
  mqttClient.loop ();
 
  // 우리가 "MakerIOTopic"주제에 가입했는지 확인하십시오.
  mqttClient.subscribe ( "MakerIOTopic");
 
  // 주제 "MakerIOTopic"에 값을 게시하려고 시도합니다.
  if (mqttClient.publish ( "MakerIOTopic", "Hello World"))
  {
    Serial.println ( "게시 메시지 성공");
  }
  else
  {
    Serial.println ( "메시지를 보낼 수 없습니다 ");
  }
 
  // 서버에 과부하를 걸지 마십시오!
  delay (4000);
}

void subscribeReceive(char* topic, byte* payload, unsigned int length)
{
  // Print the topic
  Serial.print("Topic: ");
  Serial.println(topic);
 
  // Print the message
  Serial.print("Message: ");
  for(int i = 0; i < length; i ++)
  {
    Serial.print(char(payload[i]));
  }
 
  // Print a newline
  Serial.println("");
}

