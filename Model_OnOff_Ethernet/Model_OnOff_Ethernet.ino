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

3.컴퓨터를 옴기거라 디렉토리가 변경되어서 실행이 안될경우.
C:\Users\com\Documents\GitHub\ArduinoProject\Model_OnOff_Ethernet   
이런식으로 (사용자)  디렉토리에서 pubsubclient.h  파일를 찾아 수정한다.
실질적으로 적용되는 디렉토리 파일이다.
///
 */
 
#include <ArduinoJson.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient ethernetClient;
PubSubClient client;

//const char* ethernetClientName = "Moter4_01087700060";  // 각 현장마다 다르게 한다.
const char* ethernetClientName = "testHagabi_moter";  // 각 현장마다 다르게 한다.

const char* serverIp = "119.205.235.214";

//const char* outTopicEachControl = "Moter4_01087700060/moterOutput"; // 보내는 토픽
//const char* inputTopic ="Moter4_01087700060/eachControl"; // 받는 토픽
const char* outTopicEachControl = "t/moterOutput"; // 보내는 토픽
const char* inputTopic ="t/eachControl"; // 받는 토픽

String inString ="";

//버튼 값들 저장.
String button1;// 상태(on.off & idle) 
String button2;
String button3;
String button4;
String button5;

int setPin1 = 4;
int setPin2 = 5;
int setPin3 = 6;
int setPin4 = 7;
int setPin5 = 8;

void parseCommand(String com);

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {  

 // Serial.print("Message arrived [");   //---------------------------------------------
 // Serial.print(topic);//------------------------------------------------------------------------ test 후 꼭 주석.
 // Serial.print("] ");//--------------------------------------------------
  
  // payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에 
  for (int i = 0; i < length; i++) {    
      inString += (char)payload[i];     
  } 
 // Serial.print("inString : ");
 //Serial.println(inString);
 String topics = String(topic);

  // payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에
  
 //if(topics == "Moter4_01087700060/eachControl")// 개별 제어   
 if(topics == "t/eachControl")// 개별 제어   
      parsingEachMessage(topics , inString); 

  inString = ""; //초기화.
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
       client.subscribe(inputTopic);
    }
    else 
    {
      Serial.print("failed, rc=");   
      Serial.print(client.state()); 
      Serial.println(" try again in 2 seconds");       
      delay(2000);
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
  
  pinMode(setPin1, OUTPUT);
  pinMode(setPin2, OUTPUT);
  pinMode(setPin3, OUTPUT);
  pinMode(setPin4, OUTPUT);
  pinMode(setPin5, OUTPUT);
  
  //바로 꺼준다.
  AllSetHigh();
    
  startEthernetSetting();
}

// high해야 꺼지고  low해야 켜짐
void AllSetHigh()
{
  digitalWrite(setPin1, HIGH);
  digitalWrite(setPin2, HIGH);
  digitalWrite(setPin3, HIGH);
  digitalWrite(setPin4, HIGH);
  digitalWrite(setPin5, HIGH);
}


//스위치 한개씩 제어.
void parsingEachMessage(String topics,String inString) //TODO. 형식으로 패킷만들어라 unity side: PacketEachControl
{ 
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(inString);   

  char *s = root["buttonState"];
  String buttonState = s;

  char *v = root["buttonNum"];
  int buttonNum = atoi(v);   
  
  if( parseCommand(buttonState,buttonNum)) // 에러없이 작동 잘했다면 클라이언트로 전송.
  {    
    //버튼의 상태를 전부다 보내 준다.
    char msg[200];
    StaticJsonBuffer<200> sendjsonBuffer;
    JsonObject& sendRoot = sendjsonBuffer.createObject();
    sendRoot["buttonState1"] = button1;
    sendRoot["buttonState2"] = button2;
    sendRoot["buttonState3"] = button3;
    sendRoot["buttonState4"] = button4;     
    sendRoot["buttonState5"] = button5; 
    sendRoot.printTo(msg);    
    
    client.publish(outTopicEachControl, msg,true);        
  }   
}

//프로토콜 pinOn,pinIdle,pinOff     (명령 + " " + 핀번호)
bool parseCommand(String buttonState ,int orderNum)
{
  bool isActionOk = false;
 // Serial.print("buttonState : ");//-------------------------------------------------------------------------------------------
 // Serial.println(buttonState); 

 // Serial.print("orderNum : ");//-------------------------------------------------------------------------------------------
  //Serial.println(orderNum); 
  
   int pin = changeNum(orderNum);   

   if(buttonState.equalsIgnoreCase("pinOn"))
    {   
      digitalWrite(pin, LOW);  //+   
      //digitalWrite(pin + 1, HIGH); //-     
      isActionOk = true;
    }
    /*
    else if(buttonState.equalsIgnoreCase("pinIdle") )
    {
        digitalWrite(pin, HIGH);  // high로 해야 꺼짐상태가 됨.( % 켜짐(LOW도 꺼짐상태가 되나 장시간 사용으로 릴레이 수명이 단축된다)
        digitalWrite(pin + 1, HIGH);  // high로 해야 꺼짐상태가 됨.( % 켜짐(LOW도 꺼짐상태가 되나 장시간 사용으로 릴레이 수명이 단축된다)
        isActionOk = true;
    }
    */
    else if(buttonState.equalsIgnoreCase("pinOff"))
    {   
      digitalWrite(pin, HIGH);//-   
      //digitalWrite(pin +1 , LOW);//+  
      isActionOk = true;
    } 
     else{
      Serial.println("menualy -------line:233 ");
      Serial.println(inString);
      isActionOk = false;
    }     

  

  if(isActionOk)// 작동하고 결과를 각 버튼에 저장한다.
    setButtonValue(orderNum,buttonState); 
  
  return isActionOk;
}

//작동하고 난후 결과 값을 입력.
void setButtonValue(int pinNum,String buttonState)
{
  if(pinNum == 1)
  button1 = buttonState;
  else if(pinNum == 2)
  button2 = buttonState;
  else if(pinNum == 3)
  button3 = buttonState;
  else if(pinNum == 4)
  button4 = buttonState;  
  else if(pinNum == 5)
  button5 = buttonState; 
}

//하우스 번호(클라이언트)에 마췄다.  
int changeNum(int orderNum)
{
  int order;
    if(orderNum == 1)
    order = setPin1;
    else if(orderNum == 2)
    order = setPin2;
    else if(orderNum == 3 )
    order = setPin3;
    else if(orderNum == 4 )
    order = setPin4;    
    else if(orderNum == 5 )
    order = setPin5;
    
    return order;
}

void loop() { 
  if (!client.connected()) {
    reconnect();
  }  
  client.loop();
}
