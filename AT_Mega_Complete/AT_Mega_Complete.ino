/**
 * 개요.
 * state 모드 - 클라이언트 모드로 시작!
    클라이언트 모드로 ip web server에 전송하고
    state + ap 모드로 서버 시작
 * **/

#include "ESP8266.h"

#define SSID        "KT_GiGA_2G_76C7"
#define PASSWORD    "4jf38gf684"
#define HOST_NAME   "www.hagabi.com"
#define HOST_PORT   (80)

ESP8266 wifi(Serial3,115200);

 //공용 ip 값 전달.
String readString = "";
//각각의 ip에 전달될 response
String responseMessage = "";
//close commend
String closeCommand="";
//각각의 연결 id
int connectionId;

#define DEBUG true

//서버로 부터 중복되는 (버튼) 값 처리용도.
int getValueStore = 0;

void setup(void)
{  
    Serial.begin(115200);
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());


    while (!wifi.setOprToStationSoftAP()) {
      Serial.println(wifi.restart());
      Serial.print("to station + softap err\r\n");        
    } 
    Serial.print("to station + softap ok\r\n"); 
    
      /*
    //클리이언트 모드로 시작.
    while(!wifi.setOprToStation())
    {
       Serial.println("Station Mode Error");      
    }
    Serial.println("Station Mode Ok");
 */
  //tcp만 열어라.
   while (!wifi.disableMUX()) {        
         Serial.print("single err \r\n");  
         Serial.print(wifi.restart());
         delay(1000);
    }           
    Serial.print("single ok \r\n");         
   
    //wifi 로그인.
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        //// 고정 ip 설정
        Serial3.print("AT+CIPSTA=\"172.30.1.32\",\"172.30.1.254\",\"255.255.255.0\"\r\n");
        delay(1000);      

        Serial.println("Local IPs:");
        //wifi local -> ip 확인
        Serial.println( wifi.getLocalIP().c_str());    
        
    } else {
        Serial.print("Join AP failure\r\n");
    }  

    //공용 ip 얻어오기.
    readString = GetExternalIP();
  


    //////
    // uint8_t buffer[1024] = {0};
    //tcp 연결.
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");             
    }
/*
     //통신 ip 받아놓기.
    String response = "";
    Serial3.print("AT+CIPSTA?\r\n"); // send the read character to the esp8266
    long int time = millis();
    
    while( (time+1000) > millis()) {
      while(Serial3.available()) {
        // The esp has data so display its output to the serial window 
        char c = Serial3.read(); // read the next character.
        response+=c;
      }
    }    
    
    Serial.print("test response::::");

   //받아논 ip 앞 뒤 짤라서 이쁘게 만들기.
    int ind1=response.indexOf('"');
    int ind2=response.indexOf('"',ind1 + 1);
    readString = response.substring(ind1+1,ind2);
    Serial.println(readString);
*/
  //해가비 web_server로 값 전달.//ip의 값으로 지정 주소 전달 //ip1.ip2.ip3.ip4...
   String value = "";
    value+= "GET /hagabiManager.jsp?ip2=";
    value+= readString;
    value+=" HTTP/1.1\r\nHost: www.hagabi.com\r\nConnection: close\r\n\r\n";
    char *hello = value.c_str();
    Serial.print("send:");
    Serial.println(wifi.send((const uint8_t*)hello, strlen(hello)));

   // 위쪽으로는 클라이언트 모드로 웹으로 값을 전송하는 과정 이었고
   //지금 부터는 외부로 부터 들어오는 값을 처리 하기 위해
   // 서버 모드로 전환하는 과정.
/*
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    } 
 
   while (!wifi.releaseTCP()) {
    Serial.print("release tcp err\r\n");    
    Serial.print(wifi.restart());    
    }
    Serial.print("release tcp ok\r\n");
*/
    //서버가 되기 위해서.
    // udp or tcp 다 받어라. 멀티로. 
    while(!wifi.enableMUX())
    {
      Serial.println("multiple error");   
      delay(1000);   
    }
    Serial.println("multiple ok");

    Serial.println("ip status :");
    Serial.print(wifi.getIPStatus());
    
    //서버 만든다.
    while(!wifi.startTCPServer(8090))
    {
      Serial.println(" server_not_create_error");  
       delay(1000);       
    }
    Serial.println(" server_create_ok");

     while(!wifi.setTCPServerTimeout(10)){
      Serial.print("set tcp server timout err\r\n");
     }
      Serial.print("set tcp server timout 10 seconds\r\n");

      //pin settings
      AllSetOutPutMode();

     // AllLOWSetDigitalWrite(); 
}

void loop() {
 if(Serial3.available()) { // check if the esp is sending a message
    if(Serial3.find("+IPD,")) 
    {
       delay(500); // wait for the serial buffer to fill up (read all the serial data)
       // get the connection id so that we can then disconnect
       connectionId = Serial3.read()-48; // subtract 48 because the read() function returns 
                                             // the ASCII decimal value and 0 (the first decimal number) starts at 48
       Serial3.find("pin="); // advance cursor to "pin="
       int pinNumber = (Serial3.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
       pinNumber += (Serial3.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number    
       Serial.print("pinNumber : ");
       Serial.println(pinNumber);
       
      if(getValueStore != pinNumber)
      {         
        //번호에 따른 명령.
        OrderDigitalWrite(pinNumber);
        getValueStore = pinNumber;
      } 
      else{
        Serial.println("*** reduplication pin number *** ");
      }

      responseMessage ="<h1>Hello World</h1>";
      espsend(responseMessage);       
    
      //공통 닫기.
      closeCommand = "AT+CIPCLOSE="; 
      closeCommand+=connectionId; // append connection id
      closeCommand+="\r\n";
      sendData(closeCommand,500,DEBUG); // close connection        
    }           
  }
}

  //////////////////////////////sends data from ESP to webpage///////////////////////////
 
 void espsend(String d)
{
String cipSend = " AT+CIPSEND=";
cipSend += connectionId; 
cipSend += ",";
cipSend +=d.length();
cipSend +="\r\n";
sendData(cipSend,1000,DEBUG);
sendData(d,1000,DEBUG); 
}

/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug) {
    String response = "";
    Serial3.print(command); // send the read character to the esp8266
    long int time = millis();
    
    while( (time+timeout) > millis()) {
      while(Serial3.available()) {
        // The esp has data so display its output to the serial window 
        char c = Serial3.read(); // read the next character.
        response+=c;
      }
    }
    
    if(debug) {
      Serial.print(response);
    }
    return response;
}

void OrderDigitalWrite(int orderNum)
{
  // delay(500);
  //1번(2~3)
  if(orderNum == 11){   
   digitalWrite(2, LOW);  //+
   digitalWrite(3, HIGH); //- 
  }
  if(orderNum == 12){
   digitalWrite(2, LOW);  //+
   digitalWrite(3, LOW);  //+
  }
  if(orderNum == 13){     
   digitalWrite(2, HIGH);//-  
   digitalWrite(3, LOW);//+ 
  }
  

  //2번(4~5)
  if(orderNum == 14){   
   digitalWrite(4, LOW);  //+
   digitalWrite(5, HIGH); //- 
  }
  if(orderNum == 15){
   digitalWrite(4, LOW);  //+
   digitalWrite(5, LOW);  //+
  }
  if(orderNum == 16){     
   digitalWrite(4, HIGH);//-  
   digitalWrite(5, LOW);//+ 
  }

   //3번(6~7)
 if(orderNum == 17){   
   digitalWrite(6, LOW);  //+
   digitalWrite(7, HIGH); //- 
  }
  if(orderNum == 18){
   digitalWrite(6, LOW);  //+
   digitalWrite(7, LOW);  //+
  }
  if(orderNum == 19){     
   digitalWrite(6, HIGH);//-  
   digitalWrite(7, LOW);//+ 
  }

  //4번(8~9)
  if(orderNum == 20){   
   digitalWrite(8, LOW);  //+
   digitalWrite(9, HIGH); //- 
  }
  if(orderNum == 21){
   digitalWrite(8, LOW);  //+
   digitalWrite(9, LOW);  //+
  }
  if(orderNum == 22){     
   digitalWrite(8, HIGH);//-  
   digitalWrite(9, LOW);//+ 
  }

  //5번(10~11)
 if(orderNum == 23){   
   digitalWrite(10, LOW);  //+
   digitalWrite(11, HIGH); //- 
  }
  if(orderNum == 24){
   digitalWrite(10, LOW);  //+
   digitalWrite(11, LOW);  //+
  }
  if(orderNum == 25){     
   digitalWrite(10, HIGH);//-  
   digitalWrite(11, LOW);//+ 
  }

  //6번(12~13)
  if(orderNum == 26){   
   digitalWrite(12, LOW);  //+
   digitalWrite(13, HIGH); //- 
  }
  if(orderNum == 27){
   digitalWrite(12, LOW);  //+
   digitalWrite(13, LOW);  //+
  }
  if(orderNum == 28){     
   digitalWrite(12, HIGH);//-  
   digitalWrite(13, LOW);//+ 
  }

   //7번(22~23)
  if(orderNum == 29){   
   digitalWrite(22, LOW);  //+
   digitalWrite(23, HIGH); //- 
  }
  if(orderNum == 30){
   digitalWrite(22, LOW);  //+
   digitalWrite(23, LOW);  //+
  }
  if(orderNum == 31){     
   digitalWrite(22, HIGH);//-  
   digitalWrite(23, LOW);//+ 
  }

    //8번(24~25)
if(orderNum == 32){   
   digitalWrite(24, LOW);  //+
   digitalWrite(25, HIGH); //- 
  }
  if(orderNum == 33){
   digitalWrite(24, LOW);  //+
   digitalWrite(25, LOW);  //+
  }
  if(orderNum == 34){     
   digitalWrite(24, HIGH);//-  
   digitalWrite(25, LOW);//+ 
  }

  //9번(26~27)
if(orderNum == 35){   
   digitalWrite(26, LOW);  //+
   digitalWrite(27, HIGH); //- 
  }
  if(orderNum == 36){
   digitalWrite(26, LOW);  //+
   digitalWrite(27, LOW);  //+
  }
  if(orderNum == 37){     
   digitalWrite(26, HIGH);//-  
   digitalWrite(27, LOW);//+ 
  }

   //10번(28~29)
  if(orderNum == 38){   
   digitalWrite(28, LOW);  //+
   digitalWrite(29, HIGH); //- 
  }
  if(orderNum == 39){
   digitalWrite(28, LOW);  //+
   digitalWrite(29, LOW);  //+
  }
  if(orderNum == 40){     
   digitalWrite(28, HIGH);//-  
   digitalWrite(29, LOW);//+ 
  }

  //11번(30~31)
 if(orderNum == 41){   
   digitalWrite(30, LOW);  //+
   digitalWrite(31, HIGH); //- 
  }
  if(orderNum == 42){
   digitalWrite(30, LOW);  //+
   digitalWrite(31, LOW);  //+
  }
  if(orderNum == 43){     
   digitalWrite(30, HIGH);//-  
   digitalWrite(31, LOW);//+ 
  }

  //12번(32~33 pins)
if(orderNum == 44){   
   digitalWrite(32, LOW);  //+
   digitalWrite(33, HIGH); //- 
  }
  if(orderNum == 45){
   digitalWrite(32, LOW);  //+
   digitalWrite(33, LOW);  //+
  }
  if(orderNum == 46){     
   digitalWrite(32, HIGH);//-  
   digitalWrite(33, LOW);//+ 
  } 

  //13번(34pin) 환풍기.앞
if(orderNum == 47){     
   digitalWrite(34, HIGH); //on
  }
  if(orderNum == 48){
   digitalWrite(34, LOW);  //off 
  }  

    //14번(35pin) 환풍기.뒤
if(orderNum == 49){     
   digitalWrite(35, HIGH); //on
  }
  if(orderNum == 50){
   digitalWrite(35, LOW);  //off 
  }  
}

void AllSetOutPutMode()
{
  //1번
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  //2번
  pinMode(4, OUTPUT);  
  pinMode(5, OUTPUT);
   //3번
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
   //4번
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  //5번
  pinMode(10, OUTPUT); 
  pinMode(11, OUTPUT);
  //6번
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
   //7번
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  //8번
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
   //9번
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  //10번
  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
   //11번
  pinMode(30, OUTPUT);  
  pinMode(31, OUTPUT);
   //12번
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);  

   //13번 환풍기.앞
  pinMode(34, OUTPUT);
  //14번 환풍기.뒤
  pinMode(35, OUTPUT);  
}

void AllLOWSetDigitalWrite()
{
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  
  digitalWrite(22, LOW);
  digitalWrite(23, LOW);
  digitalWrite(24, LOW);
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  digitalWrite(28, LOW);
  digitalWrite(29, LOW);
  digitalWrite(30, LOW);
  digitalWrite(31, LOW);
  digitalWrite(32, LOW);
  digitalWrite(33, LOW);  
  //환풍기 앞
  digitalWrite(34, LOW);
  //환풍기 뒤
  digitalWrite(35, LOW);  
  }

 
 String GetExternalIP()
{
    uint8_t buffer[1024] = {0};
    //tcp 연결.
    if (wifi.createTCP("api.ipify.org", 80)) {
        Serial.print("create tcp [get public ip]\r\n");
    } else {
        Serial.print("create tcp err [get public ip] \r\n");             
    }

     //통신 ip 받아놓기.
    String getPublicIpReadString = "";  
    long int time = millis();

     //해가비 web_server로 값 전달.
   String value = "";
    value+= "GET /?format=json HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n";        
    char *hello = value.c_str();
    
    Serial.println("for get public ip send:");
    Serial.println(wifi.send((const uint8_t*)hello, strlen(hello)));
    
    while( (time+1000) > millis()) {
      while(Serial3.available()) {
        // The esp has data so display its output to the serial window 
        char c = Serial3.read(); // read the next character.
        getPublicIpReadString+=c;
      }      
    }        
    
    Serial.println("Get public Ip response::::");
   //받아논 ip 앞 뒤 짤라서 이쁘게 만들기.
    int ind1=getPublicIpReadString.indexOf('"');
    int ind2=getPublicIpReadString.indexOf('"',ind1 + 1);
    int ind3=getPublicIpReadString.indexOf('"',ind2 + 1);
     int ind4=getPublicIpReadString.indexOf('"',ind3 + 1);
    getPublicIpReadString = getPublicIpReadString.substring(ind3+1,ind4);
    Serial.println(getPublicIpReadString);   
  
    //공인 Ip를 얻기 위한 연결을 끊는다.
    //해가비로 다시 tcp 연결을 위해.
    while (!wifi.releaseTCP()) {
    Serial.print("release tcp err\r\n");      
    }
    Serial.print("release tcp ok\r\n");
    //
    return getPublicIpReadString;
}
