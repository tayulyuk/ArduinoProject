#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//OTA 
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

//const char* ssid = "KT_GiGA_2G_76C7"; // 와이파이 AP, 또는 스마트폰의 핫스판 이름
//const char* password = "4jf38gf684";  // 와이파이 AP, 또는 스마트폰의 핫스판 이름

const char* mqtt_server = "119.205.235.214"; //브로커 주소

const char* outTopic = "ModelOnOff/result"; // 밖으로 내보내는 토픽.
const char* clientName = "980303Client";  // 다음 이름이 중복되지 않게 꼭 수정 바람 - 생년월일 추천

const char* setWifiManangerName = "Auto_Model_OnOff_Connect_AP";

WiFiClient espClient;
PubSubClient client(espClient);


//버튼 값들 저장.
char button1[10]="0";
char button2[10]="0";
char button3[10]="0";
char button4[10]="0";
char buttonPower[10]="0";

String inString="";
String topics ="";

char* mchar; //클라이언트로 부터 버튼의 값을 받는다. 이것으로 각 버튼에게 저장한다
char *messge;// 버튼들의 값을 모아서 클라이언트로 보내기 위한 것.

int buttonPin1 = 5;
int buttonPin2 = 4;
int buttonPin3 = 14;
int buttonPin4 = 12;
int buttonPinPower = 13;

void setup() {
  pinMode(buttonPin1, OUTPUT);
  pinMode(buttonPin2, OUTPUT);
  pinMode(buttonPin3, OUTPUT);
  pinMode(buttonPin4, OUTPUT);
  pinMode(buttonPinPower, OUTPUT);
  
  //바로 꺼준다.
  AllSetHigh();
    
  Serial.begin(115200);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around   
  WiFiManager wifiManager;

   if (!wifiManager.autoConnect(setWifiManangerName)) {
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
//-- test
//   pinMode(LED_BUILTIN, OUTPUT);
}

// high해야 꺼지고  low해야 켜짐
void AllSetHigh()
{
  digitalWrite(buttonPinPower, HIGH);
  digitalWrite(buttonPin1, HIGH);
  digitalWrite(buttonPin2, HIGH);
  digitalWrite(buttonPin3, HIGH);
  digitalWrite(buttonPin4, HIGH);
}

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
  inString = "";
  Serial.print("Message arrived [");   //---------------------------------------------
  Serial.print(topic);//------------------------------------------------------------------------ test 후 꼭 주석.
  Serial.print("] ");//--------------------------------------------------
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  //-------------------------------- 이것도 출력이네  .. print 부분 빼세요.
  }
  Serial.println();

  // payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에
  
 mchar = (char*)calloc((length + 1),sizeof(char));
 messge = (char*)calloc(110,sizeof(char));
 
  for (int i = 0; i < length; i++) {
    inString += (char)payload[i];
    mchar[i] +=(char)payload[i];
  }
  
  Serial.println("getString :" + inString); //-----------------------------------------------
 
   topics = topic;
   if(topics == "ModelOnOff/button1")  
   {
    Serial.println("button1"); //---------------------------------------------------------------------
    //각 버튼에 상태 입력하고 
    *button1 = *mchar;  
        
    //<코딩과 제어에서 혼돈할수 있는 부분. >
    //제어신호에 0을 입력시 릴레이가 ON 되며 COM공통단자와 NO단자가 연결되고 
    //제어신호에 1을 입력시 릴레이가 OFF되며 COM공통단자와 NC단자가 연결됩니다.즉, COM은 공통단자입니다.
    //0을 입력시 꺼지길 바란다.-> (버튼,HIGH)              1을 입력시 켜지길 바란다 ->(버튼, LOW )
    
    //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPin1, HIGH);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPin1, LOW);
    }
   }
   else if(topics == "ModelOnOff/button2")  
   {
    Serial.println("button2"); //----------------------------------------------------------------
    //각 버튼에 상태 입력하고 
    *button2 = *mchar;   
    
     //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPin2, HIGH);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPin2, LOW);
    }
   }
   else if(topics == "ModelOnOff/button3")  
   {
    Serial.println("button3"); //----------------------------------------------------------------
    //각 버튼에 상태 입력하고 
    *button3 = *mchar;   

     //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPin3, HIGH);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPin3, LOW);
    }
   }
   else if(topics == "ModelOnOff/button4")  
   {
    Serial.println("button4"); //---------------------------------------------------------------------
    //각 버튼에 상태 입력하고 
    *button4 = *mchar;   

     //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPin4, HIGH);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPin4, LOW);
    }
   }
   else if(topics == "ModelOnOff/buttonPower")  
   {
    Serial.println("buttonPower"); //----------------------------------------------------------------------------
    //각 버튼에 상태 입력하고 
    *buttonPower = *mchar;   

     //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPinPower, HIGH);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPinPower, LOW);
    }
   }
    //다시 클라이언트로 현재의 버튼 상황을 보낸다. & mqtt서버에 마지막 정보를 또한 저장된다. 항상클라이언트는 마지막 정보를 받는다.
    sprintf(messge,"|button1=%s|button2=%s|button3=%s|button4=%s|buttonPower=%s|",button1,button2,button3,button4,buttonPower);  
    
    //true -> retained 옵션 설정시 마지막 메시지가 broker에 큐 형태로 있다가 
    //다른 subcribe가 접속하면 큐에있던 메시지를 보낸다.-> 마지막 상태를 알수 있다.   
    client.publish(outTopic,messge,true);    
     
    free(messge);
    free(mchar);
}

// mqtt 통신에 지속적으로 접속한다.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientName)) 
    {
      Serial.println("connected");//------------------------------------------------------------------------------
      // Once connected, publish an announcement...//  클라이언트에게 다시 접속 했다고 알림.
     client.publish(outTopic, "Reconnected");
      // ... and resubscribe    
     client.subscribe("ModelOnOff/button1"); //  정리 필요. string
     client.subscribe("ModelOnOff/button2");
     client.subscribe("ModelOnOff/button3");
     client.subscribe("ModelOnOff/button4");
     client.subscribe("ModelOnOff/buttonPower");
      // 핑을 넣자.  
     client.subscribe("ModelOnOff/ping");     
    }
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying--- 5초는 너무 길어.. 1초로 바꿔.
      /*
       * 
       * 다시 컨넥트 할수 있도록 재부팅 
       * OTA 넣자.
       * 통신이 불안하여 재접속했다고 클라이언트에게 알려줄까??
       * 
       */
      delay(1000);
    }
  }
}

void loop() { 

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //--test
 //  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
 // delay(1000);                       // wait for a second
 // digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
 // delay(1000);  

  //OTA
  ArduinoOTA.handle();
}
