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

String autoControlTemp; // 전체 자동온도설정(채크한것만)    //min max 1 2 3 4  순서.
String minTemp;
String maxTemp;
String curTemp; // 실시간으로 받는 온도.
String curHumi; // 습도.
String isAutoTemp="";

String isAutoButton1 ="";
String isAutoButton2 ="";
String isAutoButton3 ="";
String isAutoButton4 ="";

const char* ethernetClientName = "1029931969033";  // 각 현장마다 다르게 한다.
const char* serverIp = "119.205.235.214";
const char* outTopic = "1029931969033/result";

void parseCommand(String com);
String sortMessage()
{
  String msg = "";
  msg ="|b1=";
  msg += button1;
  msg +="|b2=";
  msg += button2;
  msg +="|b3=";
  msg += button3;
  msg +="|b4=";
  msg += button4;
  // 오토 중이냐 유무. - 오토 설정에 대한 정보.
  msg +="|at="; //auto temp
  msg += isAutoTemp;

  msg +="|minT=";
  msg += minTemp;
  msg +="|maxT=";
  msg += maxTemp;
  
  msg +="|isAB1="; //isAutoButton1
  msg += isAutoButton1;
  msg +="|isAB2="; //isAutoButton2
  msg += isAutoButton2 ;
  msg +="|isAB3=";//isAutoButton3
  msg += isAutoButton3;
  msg +="|isAB4=";//isAutoButton4
  msg += isAutoButton4;
  msg +="|";
  msg +="\n"; // 유니티에서 읽음. 없으면 못읽음.
  return msg;
}
// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) { 
 
  //Serial.print("Message arrived [");   //---------------------------------------------
  //Serial.print(topic);//------------------------------------------------------------------------ test 후 꼭 주석.
  //Serial.print("] ");//--------------------------------------------------

// payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에 
  for (int i = 0; i < length; i++) {
    inString += (char)payload[i];     
  } 
  // Serial.print("order :");
  //Serial.println(inString);
   String topips = String(topic);
   
  if(topips == "1029931969033/button1")  
  {  
    if(isAutoTemp.equalsIgnoreCase("off") || isAutoTemp == "") //자동 온도 조절 off 일경우.
    { 
      // 들어오는 온도값에 돌려 주는 값은 없다.
       if(topips != "1029931969033/temp1") 
      {
        button1 = inString;
        parseCommand(inString,1);
      }
    }     
  }
   else if(topips == "1029931969033/button2")
   {    
    if(isAutoTemp.equalsIgnoreCase("off") || isAutoTemp == "") //자동 온도 조절 off 일경우.
    { 
      // 들어오는 온도값에 돌려 주는 값은 없다.
       if(topips != "1029931969033/temp1") 
      {
        button2 = inString;
        parseCommand(inString,2);
      }
    }     
   }
   else if(topips == "1029931969033/button3")
   {    
    if(isAutoTemp.equalsIgnoreCase("off") || isAutoTemp == "") //자동 온도 조절 off 일경우.
    { 
      // 들어오는 온도값에 돌려 주는 값은 없다.
       if(topips != "1029931969033/temp1") 
      {
        button3 = inString;
        parseCommand(inString,3);
      }
    }     
   }
   else if(topips == "1029931969033/button4") 
   {   
    if(isAutoTemp.equalsIgnoreCase("off") || isAutoTemp == "") //자동 온도 조절 off 일경우.
    { 
      // 들어오는 온도값에 돌려 주는 값은 없다.
       if(topips != "1029931969033/temp1") 
      {
        button4 = inString;
        parseCommand(inString,4);
      }
    }     
   }
  //1동의 온도 
   else if(topips == "1029931969033/temp1")  
     parsingTemp(inString); 

 //오토 온도 조절.  시작& 종료 두가지.
   else if(topips == "1029931969033/autoTemp") //자동온도 중이다..     
   {   
    autoParsing(inString);
   }
    else if(topips == "1029931969033/autoNot")    // autoOff  결과값으로 올것이다.
    isAutoTemp = inString;  
   

    if(isAutoTemp.equalsIgnoreCase("on"))
    {    
      autoTempControl();    
    }  
    /* 
    else if(isAutoTemp.equalsIgnoreCase("off") || isAutoTemp == "") //자동 온도 조절 off 일경우.
    { 
      if(topips != "1029931969033/temp1") // 들어오는 온도값에 돌려 주는 값은 없다.
        parseCommand(inString);  -------------------------------------------------------------------------------------
    }
*/
     if(topips != "1029931969033/temp1")// 들어오는 온도값에 돌려 주는 값은 없다.
     {
       SendCommand(sortMessage());         
     }
       
     inString = "";  
}

void parsingTemp(String msg)
{
   int ind1,ind2,ind3,ind4;
      ind1 = msg.indexOf('=');      
      ind2 = msg.indexOf('|', ind1+1 );       
      ind3 = msg.indexOf('=', ind2+1 );      
      ind4 = msg.indexOf('|', ind3+1 );      
     
     curTemp = msg.substring(ind1+1, ind2);
     curHumi =  msg.substring(ind3+1 ,ind4);      
}

void autoParsing(String msg)
{
  int ind1,ind2,ind3,ind4,ind5,ind6,ind7;

      ind1 = msg.indexOf('|');
      ind2 = msg.indexOf('|', ind1+1 ); 
      ind3 = msg.indexOf('|', ind2+1 );
      ind4 = msg.indexOf('|', ind3+1 );
      ind5 = msg.indexOf('|', ind4+1 );
      ind6 = msg.indexOf('|', ind5+1 );
      ind7 = msg.indexOf('|', ind6+1 );
       
     isAutoTemp = msg.substring(0, ind1);     
     minTemp = msg.substring(ind1+1, ind2);
     maxTemp = msg.substring(ind2+1, ind3);
     isAutoButton1 = msg.substring(ind3+1 ,ind4);  
     isAutoButton2 = msg.substring(ind4+1 ,ind5);
     isAutoButton3 = msg.substring(ind5+1 ,ind6); 
     isAutoButton4 = msg.substring(ind6+1, ind7);  
}

//min max 1 2 3 4  순서.
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
         button1 = "pinOff";
      }
      else if( curT >=  maxT)
      {
        autoAction(1,"pinOn");    //문 열고.
         button1 = "pinOn";
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
}

void autoAction(int orderPinNum ,String order)
{  
  int pin = changeNum(orderPinNum); 
  
  if(order.equalsIgnoreCase("pinOn"))
  {   
    digitalWrite(pin, LOW);  //+   
    digitalWrite(pin + 1, HIGH); //- 
  }
  //자동에선 멈춤이 필요 없다. 개폐기에서 리미트설정이 되어있어서.
  /*
  else if(part1.equalsIgnoreCase("pinIdle") )
  {
      digitalWrite(pin, LOW);  //+
      digitalWrite(pin + 1, LOW);  //+      
  }
  */
  else if(order.equalsIgnoreCase("pinOff"))
  {   
    digitalWrite(pin, HIGH);//-   
    digitalWrite(pin +1 , LOW);//+  
  }   
  else{
    Serial.println("auto action --- ------------   COMMAND NOT RECONGNIZED");
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
      client.subscribe("1029931969033/button1");
      client.subscribe("1029931969033/button2");
      client.subscribe("1029931969033/button3");
      client.subscribe("1029931969033/button4");
      
      client.subscribe("1029931969033/temp1");//1동 온도

      client.subscribe("1029931969033/autoTemp"); // 온도 오토  동작
      client.subscribe("1029931969033/autoNot");  // 오토 해재.     

      client.subscribe("1029931969033/ping");      
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
    AllHIGHSetDigitalWrite();
  
    startEthernetSetting();
}


void loop() { 
  if (!client.connected()) {
    reconnect();
  }  
  client.loop(); 
}


//프로토콜 pinOn,pinIdle,pinOff     (명령 + " " + 핀번호)
void parseCommand(String com ,int orderNum)
{
  //Serial.print("parseCom : ");
  //Serial.println(com);
  String part1;
  String part2;  
  part1 = com.substring(0,com.indexOf(" "));
  //part2 = com.substring(com.indexOf(" ") + 1);
  //int pin = changeNum(part2.toInt()); 
  int pin = changeNum(orderNum); 
  
  if(part1.equalsIgnoreCase("pinOn"))
  {   
    digitalWrite(pin, LOW);  //+   
    digitalWrite(pin + 1, HIGH); //- 
  }
  else if(part1.equalsIgnoreCase("pinIdle") )
  {
      digitalWrite(pin, HIGH);  // high로 해야 꺼짐상태가 됨.( % 켜짐이 되면 장시간 사용으로 릴레이 수명이 단축된다)
      digitalWrite(pin + 1, HIGH);  // high로 해야 꺼짐상태가 됨.( % 켜짐이 되면 장시간 사용으로 릴레이 수명이 단축된다)
  }
  else if(part1.equalsIgnoreCase("pinOff"))
  {   
    digitalWrite(pin, HIGH);//-   
    digitalWrite(pin +1 , LOW);//+  
  }
   else if(part1.equalsIgnoreCase("ping"))
      Serial.println("return pong");
   else if(part1.equalsIgnoreCase("off"))
      Serial.println("auto off ~");
  else{
    Serial.println("menualy -------COMMAND NOT RECONGNIZED");
    Serial.println(inString);
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

//HIGH -> 해야 전력이 안들어간 생태가 된다.
void AllHIGHSetDigitalWrite()
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
  client.publish(outTopic, cm,true);    
  free(cm);
}
