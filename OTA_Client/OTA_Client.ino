
//해가비 온도 
#include <DHT.h>
#include <ESP8266WiFi.h>

const char* ssid = "KT_GiGA_2G_76C7";
const char* password = "4jf38gf684";
const char* host = "www.hagabi.com";  //Server IP
const int   port = 80;            //Server Port
const int sleepSeconds = 300; //to save power i send the ESP to sleep

int t,h;
String url="";

#define DHTPIN D2
//#define DHTTYPE DHT22
#define DHTTYPE DHT21
 
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
 
void setup() 
{
  Serial.begin(115200);
  delay(10);
 dht.begin();
   
  WiFi.begin(ssid, password);
   
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(D0, WAKEUP_PULLUP);
}
 
void loop() 
{ 
  //-- 추가 와이파이 start
  while ( WiFi.status() != WL_CONNECTED ) 
  {   
    WiFi.begin(ssid, password);
    if (WiFi.status() == WL_CONNECTED) 
    {
     Serial.println("WiFi Re_connected");
    }
    delay(5000);
  }
  //--  추가 와이파이 end
  

   h = (int)dht.readHumidity();
   t = (int)dht.readTemperature();
  
  if (isnan(h) || isnan(t)) 
  {
  Serial.println("Failed to read from DHT sensor!");
  return;
  }  

 // Serial.println("Start http client");  //----------------------
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;    
  }
 // Serial.println("temp call"); 
  url ="/tempAndHumi.jsp?";
   url +="temp2="; 
  url+=String(t);
  url+="&";
  url+="humi2=";
  url+=String(h);
  
 // Serial.println(url);
  //This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" + 
             "Connection: close\r\n\r\n");
  delay(100);  
  
//  Serial.println("Stop http client"); //---------------------------------- 
  client.stop();
/*
  Serial.print("Temperature: ");  //-------------------------------
  Serial.print(t);
  Serial.print(" degrees Celsius Humidity: "); //--------------------------------------
  Serial.print(h);
  Serial.println(" Sending data to sql pi");  //-------------------------------------------

  Serial.println("Waiting..."); //--------------------------------------------
  // convert to microseconds
 // ESP.deepSleep(sleepSeconds * 5000);
 */
 delay(5000);
}
