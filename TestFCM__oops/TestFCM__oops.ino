//fcm test
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "KT_GiGA_2G_Wave2_CDC0";
const char* password = "4fd0dhh108";
//const char* host = "fcm.googleapis.com";  //Server IP


 HTTPClient http;
 
void setup() 
{
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password); 
   
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
void loop() 
{ 
  //-- 추가 와이파이 start
  while ( WiFi.status() != WL_CONNECTED ) 
  {
    if (WiFi.status() == WL_CONNECTED) 
    {
     Serial.println("WiFi Re_connected");
    }     
    delay(200); 
  }
  //--  추가 와이파이 end  
 
 String data = "{";
  data +=  "\"to\": \"112857130914567800192\",";
  data +=  "\"priority\": \"high\",";
  data +=  "\"notification\": {";
  data +=  "\"body\": \"asdsadsa\",";
  data +=  "\"title\" : \"Alarm\" ";
  data +=  "} }";

  http.begin("http://fcm.googleapis.com/fcm/send");
  http.addHeader("Authorization", "key=AAAA6Sehiww:APA91bF-22_RnEXyjGgFlD8wnoXSH9PpZGEWjUv1TJ81g8Oh5WSg5SbXtyObJUfEbrxLoVSFmd954GGo7zRx8x6Xm9LZp2tHbs_19mdmeKj2cJgNR4VwLqPKhGzTJpqXWSRw8Lkh-SRT");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Host", "fcm.googleapis.com");
  http.addHeader("Content-Length", String(10));
  http.POST(data);
  http.writeToStream(&Serial);
  http.end();
  Serial.println();
  
  delay(100);  
  
//  Serial.println("Stop http client"); //---------------------------------- 
//  client.flush();
 // client.stop();

 delay(2000);
}
