/**
 * @example TCPClientSingleUNO.ino
 * @brief The TCPClientSingleUNO demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.03
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "ESP8266.h"
#include <SoftwareSerial.h>

#define SSID        "KT_GiGA_2G_76C7"
#define PASSWORD    "4jf38gf684"
#define HOST_NAME   "youk123.cafe24.com"
#define HOST_PORT   (80)

SoftwareSerial mySerial(2, 3); /* RX:D3, TX:D2 */
ESP8266 wifi(mySerial);

String receiveData = "";

void setup(void)
{
    Serial.begin(9600);
    Serial.print("*** setup begin ***\r\n");
    
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
      
    while(!wifi.setOprToStationSoftAP()) {
      Serial.print("to station + softap err\r\n");        
    } 
    Serial.print("to station + softap ok\r\n");
 
    while(!wifi.joinAP(SSID, PASSWORD)) {               
               Serial.print("Join AP failure\r\n");
    }        
    Serial.print("Join AP success\r\n");
    Serial.print("IP:");
    Serial.println( wifi.getLocalIP().c_str());   
    
    while(!wifi.disableMUX()) {
      Serial.print("single err\r\n");        
    } 
    Serial.print("single ok\r\n");
    
    Serial.print("*** setup end *** \r\n");
}
 
void loop(void)
{  
    while (!wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp err\r\n");        
        }       
         Serial.print("create tcp ok\r\n");       
         
    uint8_t buffer[512] = {0};
    String value = "";
    value+= "GET /testPhp.php?orderNum=103&id=991";
   // value+= readString;
    value+=" HTTP/1.1\r\nHost: youk123.cafe24.com\r\nConnection: close\r\n\r\n";
    char *hello = value.c_str();
    //char *hello = "Hello, this is client!";
    wifi.send((const uint8_t*)hello, strlen(hello));

 
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

  /*
    while (!wifi.releaseTCP()) {
       Serial.print("release tcp err\r\n");       
    } 
     Serial.print("release tcp ok\r\n");
  */
    delay(5000);
}
     
