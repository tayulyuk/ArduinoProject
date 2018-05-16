

#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

#define SSID    "KT_GiGA_2G_76C7"
#define PASS    "4jf38gf684"
#define DST_IP  "http://youk123.cafe24.com"    
#define DEBUG true

void setup()
{
  unsigned int i=0;
  // Open Serial1 communications and wait for port to open:
  esp8266.begin(9600);
  esp8266.setTimeout(5000);
  
  Serial.begin(115200); 
  Serial.println("ESP8266 Test Arduino code, 115200\r\n");
 
  sendData("AT+RST\r\n",2000,DEBUG);        // reset module
  sendData("AT+CWMODE=1\r\n",1000,DEBUG);     // configure as station
  //1 means Station mode, 2 means AP mode, 3 means AP + Station mode
  sendData("AT+CIPMUX=0\r\n",1000,DEBUG);     //set the single connection mode  
  
  //connect to the AP 5 times
  boolean connected=false;
  for(int i=0;i<5;i++) {
    if(connectWiFi()) {
      connected = true;
      break;
    }
  }
  
  if (!connected){while(1);}          //fail to connect. hang here
  delay(2000);
  
  //print the ip addr
  sendData("AT+CIFSR\r\n",1000,DEBUG);      // get ip address
}

void loop()
{
  int i,j;
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  String cipsend = "AT+CIPSEND=";
  
  cmd += DST_IP;
  cmd += "\",80";
  cmd += "\r\n";

#if 1 
  sendData(cmd, 1000, DEBUG);
#else
  sendData(cmd, 0, DEBUG);
  if(esp8266.find("Error")) return;
#endif
  cmd = "GET /testPhp.php?orderNum=103&id=991 HTTP/1.1Host: http://youk123.cafe24.com\r\nConnection: close\r\n\r\n";
  
  cipsend += cmd.length();
  cipsend += "\r\n";
#if 1
  sendData(cipsend, 2000, DEBUG);
#else
  sendData(cipsend, 0, DEBUG);
  if(esp8266.find(">"))
  {
    Serial.println(">");
  }else
  {
    sendData("AT+CIPCLOSE\r\n", 2000, DEBUG);
    delay(1000); return;
  }
#endif
  Serial.print(cmd);
  sendData(cmd, 10000, DEBUG);
  Serial.println("================\r\n");
  delay(1000);  
}

boolean connectWiFi()
{  
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  cmd+="\r\n";

  sendData(cmd, 0, DEBUG);
  if(esp8266.find("OK"))
  {
    Serial.println("OK, Connected to WiFi.");
    return true;
  } else {
    Serial.println("Can not connect to the WiFi.");
    return false;
  }
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
