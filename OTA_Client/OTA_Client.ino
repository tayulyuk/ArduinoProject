
//해가비 온도 
#include <DHT.h>
#include <ESP8266WiFi.h>

const char* ssid = "KT_GiGA_2G_Wave2_CDC0";
const char* password = "4fd0dhh108";
const char* host = "www.hagabi.com";  //Server IP
const int   port = 80;            //Server Port
const int sleepSeconds = 300; //to save power i send the ESP to sleep

int t,h;
String url="";

//#define DHTPIN D2
#define DHTPIN 5  //pin은  esp-8266 pin 기중으로 설정하고  꼽을땐 대응되는 핀에 꼽아야 한다. wemos d1(검정색) D1핀 <-> esp 5번

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
    if (WiFi.status() == WL_CONNECTED) 
    {
     Serial.println("WiFi Re_connected");
    }      
  }
  //--  추가 와이파이 end  
   
  h = (int)dht.readHumidity();
  t = (int)dht.readTemperature();  
    
    delay(3000);
    
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
  
  //Serial.println(url);
  //This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" + 
             "Connection: close\r\n\r\n");
  delay(100);  
  
//  Serial.println("Stop http client"); //---------------------------------- 
  client.stop();

 delay(5000);
}
