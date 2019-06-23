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

#include <ArduinoJson.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

// Set the static IP address to use if the DHCP fails to assign
//IPAddress ip(192, 168, 0, 177);
//IPAddress myDns(192, 168, 0, 1);
IPAddress ip(192, 168, 0, 234);
IPAddress myDns(192, 168, 0, 2);

EthernetClient ethernetClient;
PubSubClient client;

const char* ethernetClientName = "hagabi2dong";  // 2 동 / 현장마다 다르게 한다.

const char* serverIp = "119.205.235.214";
//const char* outTopic = "Hagabi/result";
const char* outTopicEachControl = "Hagabi/2/eachControl"; //2동 제어  
const char* outTopicPlusControl = "Hagabi/2/plusControl"; //2동 plus control
const char* outTopicAutoControl = "Hagabi/2/autoControl"; //2동 auto control

String sendMessage = "";
String inString ="";

//버튼 값들 저장.
String button1;// 상태(on.off.idle) 
String button2;
String button3;
String button4;
String button5;
String button6;
String button7;
String button8;
String button9;
String button10;
String button11;
String button12;
String button13;
String button14;

//min max 1 2 3 4  순서.
String isAutoTemp ="";  // 온도에 따른 오토제어 상태 중인가  실행: on  정지:off
String isButtonsActionState = ""; // 묶음 명령 버튼 상태 저장.

String minTemp;
String maxTemp;
String curTemp; // 실시간으로 받는 온도.
String curHumi; // 습도.

String isAutoButton1 ="";// 현재  오토 & 묶음 실행 상태인가   실행: on  정지:off
String isAutoButton2 ="";
String isAutoButton3 ="";
String isAutoButton4 ="";
String isAutoButton5 ="";
String isAutoButton6 ="";
String isAutoButton7 ="";
String isAutoButton8 ="";
String isAutoButton9 ="";
String isAutoButton10 ="";
String isAutoButton11 ="";
String isAutoButton12 ="";
 //환풍기는 뺀다.

void parseCommand(String com);

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) { 
 
  Serial.print("Message arrived [");   //---------------------------------------------
  Serial.print(topic);//------------------------------------------------------------------------ test 후 꼭 주석.
  Serial.print("] ");//--------------------------------------------------

// payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에 
  for (int i = 0; i < length; i++) {    
      inString += (char)payload[i];     
  } 
  Serial.print("order :");
  Serial.println(inString);
   String topics = String(topic);

    // 오토 실행은 [1]/[2] 로 실행한다.
   if(topics == "hagabi2dong/autoControl") //오토 켤때 사용
      parsingAutoMessage(inString); // 상태 저장 및 처음 오토 실행.[1]   
   else if(topics == "hagabi2dong/plusControl")
      parsingPlusMessage(inString);
   else if(topics == "hagabi2dong/currentTempHumi") // 2동의 온도 저장과  auto 온도 개폐제어를 주기적으로 한다(온도 받을때 마다.) [2]
      parsingWorkTemp(inString);             
   else if(topics == "hagabi2dong/eachControl")// 개별 제어   
      parsingEachMessage(topics , inString);    
   else
     Serial.println("unknown  massage --  line: 113");

     inString = ""; //초기화.
}

//받은 오토 상태 저장. & 개별적인 오토 상태 ( unity:PacketAutoInfo) - 
void inputIsAutoButtonState(JsonObject& root)
{ 
  char * bs = root["isAutoTemp"];    
  isAutoTemp = bs; 
   bs = root["isButtonsActionState"];  // 올릴 건지 내릴건지
  isButtonsActionState = bs;
  bs = root["minTemp"];
  minTemp = bs; 
   bs = root["maxTemp"];
  maxTemp = bs;     
  bs = root["isAutoButton1"];    
  isAutoButton1 = bs;
  
  bs = root["isAutoButton2"];    
  isAutoButton2 = bs;

  bs = root["isAutoButton3"];    
  isAutoButton3 = bs;

   bs = root["isAutoButton4"];    
  isAutoButton4 = bs;

  bs = root["isAutoButton5"];    
  isAutoButton5 = bs;

   bs = root["isAutoButton6"];    
  isAutoButton6 = bs;

  bs = root["isAutoButton7"];    
  isAutoButton7 = bs;

   bs = root["isAutoButton8"];    
  isAutoButton8 = bs;

  bs = root["isAutoButton9"];    
  isAutoButton9 = bs;

   bs = root["isAutoButton10"];    
  isAutoButton10 = bs;

  bs = root["isAutoButton11"];    
  isAutoButton11 = bs;

   bs = root["isAutoButton12"];    
  isAutoButton12 = bs;

}

//각동의 정보를 저장한다.   %오토 상태라면 주기적으로 오토 제어 실행 역활을 한다.
void parsingWorkTemp(String inString) // TODO. 패킷을 이형태로 만들어야 한다
{
  StaticJsonBuffer<100> jsonBuffer;  // 용량이 적어서 줄여봤다.
  JsonObject& root = jsonBuffer.parseObject(inString);
  const char * v = root["curTemp"];
  curTemp = v;
  v = root["curHumi"];
  curHumi = v;

  if(isAutoTemp.equalsIgnoreCase("on"))  // 주기적으로 실행.
    autoTempControl();
}
// 묶음 제어 ( ex:   1,4,6 번 개폐기 작동 ...)
void parsingPlusMessage(String inString)
{
  StaticJsonBuffer<500> jsonBuffer; // 받는 사이즈 440 정도.
  JsonObject& root = jsonBuffer.parseObject(inString);
  inputIsAutoButtonState(root);// 묶음 동작할 상태를 입력.

  if(isAutoButton1.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,1);
  if(isAutoButton2.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,2);
  if(isAutoButton3.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,3);
  if(isAutoButton4.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,4);
  if(isAutoButton5.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,5);
  if(isAutoButton6.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,6);
  if(isAutoButton7.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,7);
  if(isAutoButton8.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,8);
  if(isAutoButton9.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,9);
  if(isAutoButton10.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,10);
  if(isAutoButton11.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,11);
  if(isAutoButton12.equalsIgnoreCase("on"))
    parseCommand(isButtonsActionState,12);

   //다시 전송.
   String msg = getAutoButtonState(root);
    char* msgChar = msg.c_str();
    client.publish(outTopicPlusControl, msgChar,true);    
}

//오토 켜기 전용.
void parsingAutoMessage(String inString)
{
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(inString);
  inputIsAutoButtonState(root);// 오토 상태를 입력.

  autoTempControl();    
  
  //다시 전송.
   String msg = getAutoButtonState(root);
    char* msgChar = msg.c_str();
    client.publish(outTopicAutoControl, msgChar,true);    
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
    char msg[100];
    StaticJsonBuffer<100> sendjsonBuffer;
    JsonObject& sendRoot = sendjsonBuffer.createObject();
    sendRoot["buttonState"] = root["buttonState"];
    sendRoot["buttonNum"] = root["buttonNum"];    
    sendRoot.printTo(msg);    
    
    client.publish(outTopicEachControl, msg,true);       
  }   
}

//받은 정보를 다시 json 형식으로 String 문자열을 만든다.
String getAutoButtonState(JsonObject& root)
{
  char msg[500];
   StaticJsonBuffer<500> sendjsonBuffer;
    JsonObject& sendRoot = sendjsonBuffer.createObject();   
    sendRoot["isAutoTemp"] = root["isAutoTemp"];
    sendRoot["isButtonsActionState"] = root["isButtonsActionState"];    
    sendRoot["minTemp"] = root["minTemp"];
    sendRoot["maxTemp"] = root["maxTemp"]; 
    sendRoot["isAutoButton1"] = root["isAutoButton1"];
    sendRoot["isAutoButton2"] = root["isAutoButton2"];    
    sendRoot["isAutoButton3"] = root["isAutoButton3"];
    sendRoot["isAutoButton4"] = root["isAutoButton4"]; 
    sendRoot["isAutoButton5"] = root["isAutoButton5"];
    sendRoot["isAutoButton6"] = root["isAutoButton6"];    
    sendRoot["isAutoButton7"] = root["isAutoButton7"];
    sendRoot["isAutoButton8"] = root["isAutoButton8"];   
    sendRoot["isAutoButton9"] = root["isAutoButton9"];
    sendRoot["isAutoButton10"] = root["isAutoButton10"];    
    sendRoot["isAutoButton11"] = root["isAutoButton11"];
    sendRoot["isAutoButton12"] = root["isAutoButton12"];      
    sendRoot.printTo(msg);    
   return msg;
}
//min max 1 2 3 4  순서. -- 문닫/문열 ->동작 정보를 개별로 저장한다
void autoTempControl()
{ 
  char buf[5]={0};
  curTemp.toCharArray(buf,5);
  float curT=atof(buf); 

  char buf1[5]={0};
  minTemp.toCharArray(buf1,5);
  float minT=atof(buf1); 

  char buf2[5]={0};
  maxTemp.toCharArray(buf2,5);
  float maxT=atof(buf2);   

   if(isAutoButton1.equalsIgnoreCase("on"))
   {
      if( curT <=   minT)
      {
        autoAction(1,"pinOff");  // 문 닫고.
         button1 = "pinOff";      //동작 정보를 개별로 저장한다
      }
      else if( curT >=  maxT)
      {
        autoAction(1,"pinOn");    //문 열고.
         button1 = "pinOn";        //동작 정보를 개별로 저장한다
      }
   }   

    if(isAutoButton2.equalsIgnoreCase("on"))
    {
       if( curT <=  minT)
       {
        autoAction(2,"pinOff");
         button2 = "pinOff";
       }
        else if(curT >=  maxT)
        {
        autoAction(2,"pinOn");    
         button2 = "pinOn";
        }
    }
    
    if(isAutoButton3.equalsIgnoreCase("on"))
    {
       if(curT <=  minT)
       {
        autoAction(3,"pinOff");
         button3 = "pinOff";
       }
       else if(curT >= maxT)
       {
        autoAction(3,"pinOn");  
         button3 = "pinOn";
       }  
    }
    if(isAutoButton4.equalsIgnoreCase("on"))
    {
       if(curT <=  minT)
       {
        autoAction(4,"pinOff");
         button4 = "pinOff";
       }       
       else if(curT >=  maxT)
       {
        autoAction(4,"pinOn");    
        button4 = "pinOn";
       }
  }
   if(isAutoButton5.equalsIgnoreCase("on"))
   {
      if( curT <=   minT)
      {
        autoAction(5,"pinOff");  // 문 닫고.
         button5 = "pinOff";
      }
      else if( curT >=  maxT)
      {
        autoAction(5,"pinOn");    //문 열고.
         button5 = "pinOn";
      }
   }   

    if(isAutoButton6.equalsIgnoreCase("on"))
    {
       if( curT <=  minT)
       {
        autoAction(6,"pinOff");
         button6 = "pinOff";
       }
        else if(curT >=  maxT)
        {
        autoAction(6,"pinOn");    
         button6 = "pinOn";
        }
    }
    
    if(isAutoButton7.equalsIgnoreCase("on"))
    {
       if(curT <=  minT)
       {
        autoAction(7,"pinOff");
         button7 = "pinOff";
       }
       else if(curT >= maxT)
       {
        autoAction(7,"pinOn");  
         button7 = "pinOn";
       }  
    }
    if(isAutoButton8.equalsIgnoreCase("on"))
    {
       if(curT <=  minT)
       {
        autoAction(8,"pinOff");
         button8 = "pinOff";
       }       
       else if(curT >=  maxT)
       {
        autoAction(8,"pinOn");    
        button8 = "pinOn";
       }
  }   
   if(isAutoButton9.equalsIgnoreCase("on"))
   {
      if( curT <=   minT)
      {
        autoAction(9,"pinOff");  // 문 닫고.
         button9 = "pinOff";
      }
      else if( curT >=  maxT)
      {
        autoAction(9,"pinOn");    //문 열고.
         button9 = "pinOn";
      }
   }   

    if(isAutoButton10.equalsIgnoreCase("on"))
    {
       if( curT <=  minT)
       {
        autoAction(10,"pinOff");
         button10 = "pinOff";
       }
        else if(curT >=  maxT)
        {
        autoAction(10,"pinOn");    
         button10 = "pinOn";
        }
    }
    
    if(isAutoButton11.equalsIgnoreCase("on"))
    {
       if(curT <=  minT)
       {
        autoAction(11,"pinOff");
         button11 = "pinOff";
       }
       else if(curT >= maxT)
       {
        autoAction(11,"pinOn");  
         button11 = "pinOn";
       }  
    }
    if(isAutoButton12.equalsIgnoreCase("on"))
    {
       if(curT <=  minT)
       {
        autoAction(12,"pinOff");
         button12 = "pinOff";
       }       
       else if(curT >=  maxT)
       {
        autoAction(12,"pinOn");    
        button12 = "pinOn";
       }
    }           
}

void autoAction(int orderPinNum ,String order)
{  
  int pin = changeNum(orderPinNum); 
  
  if(order.equalsIgnoreCase("pinOn"))
  {   
    digitalWrite(pin, LOW);  //+   
    digitalWrite(pin + 1, HIGH); //- 
  }
  //자동에선 멈춤이 필요 없지만 /  묶음 개폐기에선 필요. 
  else if(order.equalsIgnoreCase("pinIdle") )
  {
      digitalWrite(pin, HIGH);  //+
      digitalWrite(pin + 1, HIGH);  //+      
  }
 
  else if(order.equalsIgnoreCase("pinOff"))
  {   
    digitalWrite(pin, HIGH);//-   
    digitalWrite(pin +1 , LOW);//+  
  }   
  else{
    Serial.println("auto action --- ------------  line : 488");
  } 
}

// mqtt 통신에 지속적으로 접속한다.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
   //Serial.println("Attempting MQTT connection... ?  ..........");  
    if (client.connect(ethernetClientName)) 
    {
  // Serial.println("connected");//---------------------------------------------   
      client.subscribe("hagabi1dong/autoControl");
      client.subscribe("hagabi1dong/autoControlOff");
      client.subscribe("hagabi1dong/plusControl");
      client.subscribe("hagabi1dong/eachControl");   
      client.subscribe("hagabi1dong/currentTemp1");
      client.subscribe("hagabi1dong/autoState"); // 단순 오토 유무만 주고 받음.
      client.subscribe("hagabi1dong/ping"); 
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
bool parseCommand(String buttonState ,int orderNum)
{
  bool isActionOk = false;
  Serial.print("buttonState : ");//-------------------------------------------------------------------------------------------
  Serial.println(buttonState); 

  Serial.print("orderNum : ");//-------------------------------------------------------------------------------------------
  Serial.println(orderNum); 
  
   int pin = changeNum(orderNum); 

  if(orderNum == 13 || orderNum == 14)
  {
     if(buttonState.equalsIgnoreCase("pinOn"))
      {   
        digitalWrite(pin, LOW);  //+   켬.
        isActionOk = true;
      }
      else if(buttonState.equalsIgnoreCase("pinOff"))
      {   
        digitalWrite(pin, HIGH);//-  끔      
        isActionOk = true;
      }
  }
  else
  {
     if(buttonState.equalsIgnoreCase("pinOn"))
      {   
        digitalWrite(pin, LOW);  //+   
        digitalWrite(pin + 1, HIGH); //-     
        isActionOk = true;
      }
      else if(buttonState.equalsIgnoreCase("pinIdle") )
      {
          digitalWrite(pin, HIGH);  // high로 해야 꺼짐상태가 됨.( % 켜짐(LOW도 꺼짐상태가 되나 장시간 사용으로 릴레이 수명이 단축된다)
          digitalWrite(pin + 1, HIGH);  // high로 해야 꺼짐상태가 됨.( % 켜짐(LOW도 꺼짐상태가 되나 장시간 사용으로 릴레이 수명이 단축된다)
          isActionOk = true;
      }
      else if(buttonState.equalsIgnoreCase("pinOff"))
      {   
        digitalWrite(pin, HIGH);//-   
        digitalWrite(pin +1 , LOW);//+  
        isActionOk = true;
      } 
       else{
        Serial.println("menualy -------line:612 ");
        Serial.println(inString);
        isActionOk = false;
      }     
  } 
  

  if(isActionOk)// 작동하고 결과를 각 버튼에 저장한다.
    setButtonValue(pin,buttonState); 
  
  return isActionOk;
}

//작동하고 난후 결과 값을 입력.
void setButtonValue(int pinNum,String buttonState)
{
  if(pinNum == 1)
  button1 = buttonState;
  if(pinNum == 2)
  button2 = buttonState;
  if(pinNum == 3)
  button3 = buttonState;
  if(pinNum == 4)
  button4 = buttonState;
  if(pinNum == 5)
  button5 = buttonState;
  if(pinNum == 6)
  button6 = buttonState;
  if(pinNum == 7)
  button7 = buttonState;
  if(pinNum == 8)
  button8 = buttonState;
  if(pinNum == 9)
  button9 = buttonState;
  if(pinNum == 10)
  button10 = buttonState;
  if(pinNum == 11)
  button11 = buttonState;
  if(pinNum == 12)
  button12 = buttonState;
  if(pinNum == 13)
  button13 = buttonState;
  if(pinNum == 14)
  button14 = buttonState;
}
//하우스 번호(클라이언트)에 마췄다.
int changeNum(int orderNum)
{
  int order;
    if(orderNum == 1)
    order = 22;
    else if(orderNum == 2)
    order = 24;
    else if(orderNum == 3 )
    order = 26;
    else if(orderNum == 4 )
    order = 28;    
    else if(orderNum == 5)
    order = 30;
    else if(orderNum == 6)
    order = 32;
    else if(orderNum == 7 )
    order = 34;
    else if(orderNum == 8)
    order = 36;  
    else if(orderNum == 9 )
    order = 38;  
    else if(orderNum == 10)
    order = 40;
    else if(orderNum == 11)
    order = 42;
    else if(orderNum == 12 )
    order = 44;
    else if(orderNum == 13 ) //환풍기. - 홀수
    order = 46; 
    else if(orderNum == 14 )//환풍기.-홀수 제어.
    order = 47;      
    
    return order;
}

void AllSetOutPutMode()
{
  for(int i = 22; i <= 49 ; i++)
  {    
      pinMode(i, OUTPUT);
  }
}

void AllLOWSetDigitalWrite()
{
  for(int i = 22; i <= 49 ; i++)
  {     
    digitalWrite(i, HIGH);       
  }
}
/*
//클라이언트에 받은 값을 돌려 보낸다.
void SendCommand(String msg)
{
  char* cm = (char*)calloc((msg.length() + 1),sizeof(char)); 
  msg.toCharArray(cm,msg.length()); 
  client.publish(outTopic, cm,true);    
  free(cm);
}
*/

