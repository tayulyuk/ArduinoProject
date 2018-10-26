#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>

IPAddress server(119,205,235,214);
//const char * server = "119.205.235.214"; 
char ssid[] = "KT_GiGA_2G_76C7";           // your network SSID (name)
char pass[] = "4jf38gf684";           // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status
char* topic ="siheung/namu/powerAndMoters";
char* outTopic ="siheung/namu/result";
// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(3,2); // RX, TX

//버튼들
char* button1 = "off";
char* button2 = "off";
char* button3 = "off";
char* button4 = "off";
char* buttonPower = "off";  
void (* resetFunc)(void) =0;

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);

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
  client.setServer(server, 1883);
  client.setCallback(callback);
}

//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String _string = "";
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    _string += (char)payload[i];
  }   
 Serial.println("str1:" + _string);  
   String tops = topic;
   //버튼 1번 토픽이 들어왔을경우
  if(tops == "siheung/namu/button1")
  {
    Serial.println("button1");
    //각 버튼에 상태 입력하고
  // button1 = _string.c_str();  
    //브로커에게 다시 전송한다. 

    char messge[60];       
    sprintf(messge,"%s%s%s%s%s%s%s%s%s%s%s","|button1=",button1,"|button2=",button2,"|button3=",button3,"|button4=",button4,"|buttonPower=",buttonPower,"|");
    //puts(messge);
    client.publish(outTopic,messge,true);  //true -> retained 옵션 설정시 마지막 메시지가 broker에 큐 형태로 있다가
    //다른 subcribe가 접속하면 큐에있던 메시지를 보낸다.-> 마지막 상태를 알수 있다.

      
  }
   //버튼 1번 토픽이 들어왔을경우
  if(tops == "siheung/namu/button2")
  {
    Serial.println("button2");
    //각 버튼에 상태 입력하고
    //브로커에게 다시 전송한다. 
  } 
   Serial.println("str2:" + _string);  
  Serial.println("tops:" + tops);  
  Serial.println("str3:" + _string);  
  /*
  char c[10];
  sprintf(c,"%s",button1);
  for (int i=0;i<10;i++) {
    Serial.print((char)c[i]);
  }
 
  Serial.println(c);
*/
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement... 
    char messge[60];       
    sprintf(messge,"%s%s%s%s%s%s%s%s%s%s","|button1=",button1,"|button2=",button2,"|button3=",button3,"|button4=",button4,"|buttonPower=",buttonPower,"|");
    //puts(messge);
      client.publish(outTopic,messge,true);  //true -> retained 옵션 설정시 마지막 메시지가 broker에 큐 형태로 있다가
      //다른 subcribe가 접속하면 큐에있던 메시지를 보낸다.-> 마지막 상태를 알수 있다.

      
      
      // ... and resubscribe
      client.subscribe(topic);
       client.subscribe("siheung/namu/button1");
        client.subscribe("siheung/namu/button2");

        
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying    
     resetFunc(); 
      delay(5000);          
    }
  }
}
 
