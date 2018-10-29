#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// 아래의 6개설정은 사용자 환경에 맞게 수정하세요.
//const char* ssid = "KT_GiGA_2G_76C7"; // 와이파이 AP, 또는 스마트폰의 핫스판 이름
//const char* password = "4jf38gf684";  // 와이파이 AP, 또는 스마트폰의 핫스판 이름
const char* ssid = "YoukLeeIpTimeA1004"; // 와이파이 AP, 또는 스마트폰의 핫스판 이름
const char* password = "123456789000";  // 와이파이 AP, 또는 스마트폰의 핫스판 이름
const char* mqtt_server = "119.205.235.214"; //브로커 주소

//const char* outTopic = "siheung/namu/button1"; // 밖으로 내보내는 토픽.
const char* outTopic = "siheung/namu/result"; // 밖으로 내보내는 토픽.
//const char* inTopic = "siheung/namu/order"; // 외부에서 들어오는 토픽.
const char* clientName = "980303Client";  // 다음 이름이 중복되지 않게 꼭 수정 바람 - 생년월일 추천

WiFiClient espClient;
PubSubClient client(espClient);
//long lastMsg = 0;
//char msg[100];
/*
//int led=4; // D2 GPIO4 핀을 사용
int led=BUILTIN_LED; // D1 mini에 있는 led를 사용
int timeIn=1000;  // led가 깜박이는 시간을 mqtt 통신에서 전달받아 저장
*/

//버튼 값들 저장.
char button1[10]="0";
char button2[10]="0";
char button3[10]="0";
char button4[10]="0";
char buttonPower[10]="0";

//void (* resetFunc)(void) =0;


String inString="";
String topis ="";

char* mchar; //클라이언트로 부터 버튼의 값을 받는다. 이것으로 각 버튼에게 저장한다
char *messge;// 버튼들의 값을 모아서 클라이언트로 보내기 위한 것.
/*
int buttonPin1 = D3;
int buttonPin2 = D4;
int buttonPin3 = D5;
int buttonPin4 = D6;
int buttonPinPower = D7;
*/
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
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
  inString = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에
  
 mchar = (char*)calloc((length + 1),sizeof(char));
 messge = (char*)calloc(110,sizeof(char));
 
  for (int i = 0; i < length; i++) {
    inString += (char)payload[i];
    mchar[i] +=(char)payload[i];
  }
  
  Serial.println("getString :" + inString);
 
   topis = topic;
   if(topis == "siheung/namu/button1")  
   {
    Serial.println("button1");
    //각 버튼에 상태 입력하고 
    *button1 = *mchar;       
    //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPin1, LOW);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPin1, HIGH);
    }
   }
    if(topis == "siheung/namu/button2")  
   {
    Serial.println("button2");
    //각 버튼에 상태 입력하고 
    *button2 = *mchar;   
    
     //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPin2, LOW);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPin2, HIGH);
    }
   }
    if(topis == "siheung/namu/button3")  
   {
    Serial.println("button3");
    //각 버튼에 상태 입력하고 
    *button3 = *mchar;   

     //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPin3, LOW);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPin3, HIGH);
    }
   }
    if(topis == "siheung/namu/button4")  
   {
    Serial.println("button4");
    //각 버튼에 상태 입력하고 
    *button4 = *mchar;   

     //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPin4, LOW);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPin4, HIGH);
    }
   }
    if(topis == "siheung/namu/buttonPower")  
   {
    Serial.println("buttonPower");
    //각 버튼에 상태 입력하고 
    *buttonPower = *mchar;   

     //버튼의 결과가 0이면 끔/1이면 켬
    if(inString == "0") //끔
    {
     digitalWrite(buttonPinPower, LOW);
    }
    if(inString == "1")//켬
    {
      digitalWrite(buttonPinPower, HIGH);
    }
   }
   //다시 클라이언트로 현재의 버튼 상황을 보낸다. & mqtt서버에 마지막 정보를 또한 저장된다. 항상클라이언트는 마지막 정보를 받는다.
    sprintf(messge,"|button1=%s|button2=%s|button3=%s|button4=%s|buttonPower=%s|",button1,button2,button3,button4,buttonPower);   
    client.publish(outTopic,messge,true);  //true -> retained 옵션 설정시 마지막 메시지가 broker에 큐 형태로 있다가
  // sprintf(messge,"%s",button1);
   //client.publish(outTopic,messge,true); 
    //다른 subcribe가 접속하면 큐에있던 메시지를 보낸다.-> 마지막 상태를 알수 있다.      
    free(messge);
    free(mchar);
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
     client.subscribe("siheung/namu/button1");
     client.subscribe("siheung/namu/button2");
     client.subscribe("siheung/namu/button3");
     client.subscribe("siheung/namu/button4");
     client.subscribe("siheung/namu/buttonPower");
        
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
/*
  // 들어온 timeIn 값에 따라 led가 점멸하게 한다.
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(timeIn);                       // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(timeIn); 
  */
}
