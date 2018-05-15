//비닐 하우스 버튼을 이용하여 올리고 내리고 를 작동한다 ㅎㅎ
#include <SoftwareSerial.h>
#define DEBUG true
 
SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                                        // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                                        // and the RX line from the esp to the Arduino's pin 3
 
void setup() {
  Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different
  
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
   
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
   sendData("AT+GMR\r\n",2000,DEBUG); // module vision
   
  sendData("AT+CIOBAUD?\r\n",2000,DEBUG); // check baudrate (redundant)
  sendData("AT+CWMODE=1\r\n",1000,DEBUG); // configure as access point (working mode: AP+STA)
  sendData("AT+CWLAP\r\n",3000,DEBUG); // list available access points
  sendData("AT+CWJAP=\"KT_GiGA_2G_76C7\",\"4jf38gf684\"\r\n",6000,DEBUG); // join the access point
  
  sendData("AT+CIPMUX=0\r\n",1000,DEBUG); // configure for multiple connections
  
  sendData("AT+CIPSTART=\"TCP\",\"www.hagabi.com\",80\r\n",1000,DEBUG); // 연결.
  String message = "GET/tempAndHumi.jsp?Temp=37&Humi=47HTTP/1.1\r\n";
  
  int messageLength = message.length();

  String sendMessage = "AT+CIPSEND=";
  sendMessage+= String(messageLength);
  
   //esp8266.print(sendMessage);
   sendData(sendMessage,5000,DEBUG);
  sendData(message,5000,DEBUG);
  
}
 
void loop() {
 
}
 
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug) {
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    
    while( (time+timeout) > millis()) {
      while(esp8266.available()) {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }
    }
    
    if(debug) {
      Serial.print(response);
    }
    return response;
}

