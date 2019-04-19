
//통신으로 받은 값을 메가로 전송만 한다

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

// 아래의 6개설정은 사용자 환경에 맞게 수정하세요.
//const char* ssid = "KT_GiGA_2G_76C7"; // 와이파이 AP, 또는 스마트폰의 핫스판 이름
//const char* password = "4jf38gf684";  // 와이파이 AP, 또는 스마트폰의 핫스판 이름

const char* mqtt_server = "119.205.235.214"; //브로커 주소
// 이 주소는 GUID 활용하자//1동 -> Hagabi_1_Auto_AP_Setting   2동 -> Hagabi_2_Auto_AP_Setting
const char* wifi_name ="Hagabi_2_Auto_AP_Setting";

const char* outTopic = "Hagabi/result"; // 밖으로 내보내는 토픽. -> 1,2동 같은 결과로 받는다.
// GUID를 활용하자.test1 ->1동  test2 -> 2동
const char* subscribeName = "Hagabi/test2";

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
    
  //wifiManager.autoConnect("Auto_Connect_AP");
  wifiManager.autoConnect(wifi_name);
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
  /*
  inString = "";  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  */
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  //작업할거
//////// 들어온 값이 이거라면   이렇게 write 써라  
///메가로 신호를 보내고
/// 다시 받은 값을 비교해서 같다면
////다음 작업을 해라 


  // payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에
  /*
 mchar = (char*)calloc((length + 1),sizeof(char));
 messge = (char*)calloc(110,sizeof(char));
 
  for (int i = 0; i < length; i++) {
    inString += (char)payload[i];
    mchar[i] +=(char)payload[i];
  }
  
  //Serial.println("getString :" + inString);
 Serial.println(inString);
   topis = topic;
   if(topis == "ModelOnOff/button1")  
   {
    
   }    
   //Serial.println("topis :" + topis);
   */
   /*
   //다시 클라이언트로 현재의 버튼 상황을 보낸다. & mqtt서버에 마지막 정보를 또한 저장된다. 항상클라이언트는 마지막 정보를 받는다.
    sprintf(messge,"|button1=%s|button2=%s|button3=%s|button4=%s|buttonPower=%s|",button1,button2,button3,button4,buttonPower);   
    client.publish(outTopic,messge,true);  //true -> retained 옵션 설정시 마지막 메시지가 broker에 큐 형태로 있다가
    */
  // sprintf(messge,"%s",button1);
   //client.publish(outTopic,messge,true); 
    //다른 subcribe가 접속하면 큐에있던 메시지를 보낸다.-> 마지막 상태를 알수 있다.     
    /* 
    free(messge);
    free(mchar);
    */
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

      // GUID를 활용하자.
     client.subscribe(subscribeName);
     //client.subscribe("ModelOnOff/button2");
     //client.subscribe("ModelOnOff/button3");
     //client.subscribe("ModelOnOff/button4");
     //client.subscribe("ModelOnOff/buttonPower");
        
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
}

//클라이언트에 받은 값을 돌려 보낸다.
void SendCommand(String msg)
{
  char* cm = (char*)calloc((msg.length() + 1),sizeof(char));
  //char* cm = (char*)calloc(110,sizeof(char));
  msg.toCharArray(cm,msg.length());
 // sprintf(cm,"%s",c);   
  //char ch[32] = {0};
  //msg.toCharArray(ch,msg.length());
    client.publish(outTopic, cm);
    
    free(cm);
}

