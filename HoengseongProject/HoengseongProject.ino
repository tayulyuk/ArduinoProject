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

String sendMessage = "";
String inString ="";

//버튼 값들 저장.
String button1;// 상태(on.off.idle) ,오토 채크유무.
String button2;
String button3;
String button4;
String autoTemp; // 전체 자동온도설정(채크한것만)   (autoTempOn / autoTempOff ) 최저온도, 최고온도 

char* mchar; //클라이언트로 부터 버튼의 값을 받는다. 이것으로 각 버튼에게 저장한다
char *message;// 버튼들의 값을 모아서 클라이언트로 보내기 위한 것.

const char* ethernetClientName = "1029931969033";  // 각 현장마다 다르게 한다.
const char* serverIp = "119.205.235.214";
const char* outTopic = "1029931969033/result";

void parseCommand(String com);
String sortMessage()
{
  String msg = "";
  msg +="|";
  msg = button1;
  msg +="|";
  msg += button2;
  msg +="|";
  msg += button3;
  msg +="|";
  msg += button4;
  msg +="|";
  msg +="\n";
  return msg;
}
// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) { 
 
  Serial.print("Message arrived [");   //---------------------------------------------
  Serial.print(topic);//------------------------------------------------------------------------ test 후 꼭 주석.
  Serial.print("] ");//--------------------------------------------------

// payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에
  
 mchar = (char*)calloc((length + 1),sizeof(char));
 message = (char*)calloc(110,sizeof(char));
 
  for (int i = 0; i < length; i++) {
    inString += (char)payload[i]; 
     mchar[i] +=(char)payload[i];
  } 
   
   String topips = String(topic);
   
  if(topips == "1029931969033/button1")    
  button1 = inString;
   else if(topips == "1029931969033/button2")    
  button2 = inString;
   else if(topips == "1029931969033/button3")    
  button3 = inString;
   else if(topips == "1029931969033/button4")    
  button4 = inString; 
   else if(topips == "1029931969033/autoTemp")    
  autoTemp = inString; 

 parseCommand(inString);  
 
 SendCommand(sortMessage());  
 inString = "";  
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
      client.subscribe("1029931969033/button1");
      client.subscribe("1029931969033/button2");
      client.subscribe("1029931969033/button3");
      client.subscribe("1029931969033/button4");
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
  
   //pin settings
    AllSetOutPutMode();  
    //일단 HIGH 모드로 모두 끄고 시작한다.
    AllLOWSetDigitalWrite();
  
  startEthernetSetting();
}


void loop() { 
  if (!client.connected()) {
    reconnect();
  }  
  client.loop(); 
}


//프로토콜 pinOn,pinIdle,pinOff     (명령 + " " + 핀번호)
void parseCommand(String com)
{
  String part1;
  String part2;  
  part1 = com.substring(0,com.indexOf(" "));
  part2 = com.substring(com.indexOf(" ") + 1);
  int pin = changeNum(part2.toInt()); 
  
  if(part1.equalsIgnoreCase("pinOn"))
  {   
    digitalWrite(pin, LOW);  //+   
    digitalWrite(pin + 1, HIGH); //- 
  }
  else if(part1.equalsIgnoreCase("pinIdle") )
  {
      digitalWrite(pin, LOW);  //+
      digitalWrite(pin + 1, LOW);  //+      
  }
  else if(part1.equalsIgnoreCase("pinOff"))
  {   
    digitalWrite(pin, HIGH);//-   
    digitalWrite(pin +1 , LOW);//+  
  }   
  else{
    Serial.println("COMMAND NOT RECONGNIZED");
  } 
}

//하우스 번호(클라이언트)에 마췄다.
int changeNum(int orderNum)
{
  int order;
  if(orderNum == 1)
    order = 40;
    if(orderNum == 2)
    order = 42;
    if(orderNum == 3 )
    order = 44;
    if(orderNum == 4 )
    order = 46;    
    return order;
}

void AllSetOutPutMode()
{
  for(int i = 40; i <= 47 ; i++)
  {    
      pinMode(i, OUTPUT);            
  }
}

void AllLOWSetDigitalWrite()
{
  for(int i = 40; i <= 47 ; i++)
  {     
    digitalWrite(i, HIGH);       
  }
}

//클라이언트에 받은 값을 돌려 보낸다.
void SendCommand(String msg)
{
  char* cm = (char*)calloc((msg.length() + 1),sizeof(char)); 
  msg.toCharArray(cm,msg.length()); 
  client.publish(outTopic, cm);    
  free(cm);
}


