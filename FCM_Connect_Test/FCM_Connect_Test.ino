#include <ESP8266WiFi.h>

const char* fcmServer = "https://fcm.googleapis.com";
bool sent = false;
 WiFiClient client;
void setup() {
  Serial.begin(9600);
 
  delay(1000);
  Serial.printf("\nTry connecting to WiFi with SSID '%s'\n", WiFi.SSID().c_str());

  //WiFi.mode(WIFI_STA);
  WiFi.begin(WiFi.SSID().c_str(),WiFi.psk().c_str()); // reading data from EPROM, last saved credentials
  while (WiFi.status() == WL_DISCONNECTED) {
    delay(500);
    Serial.print(".");
  }

  wl_status_t status = WiFi.status();
  if(status == WL_CONNECTED) {
    Serial.printf("\nConnected successfull to SSID '%s'\n", WiFi.SSID().c_str());
  } else {
    Serial.printf("\nCould not connect to WiFi. state='%d'", status); 
    Serial.println("Please press WPS button on your router.\n Press any key to continue...");
    while(!Serial.available()) { ; }
    if(!startWPSPBC()) {
       Serial.println("Failed to connect with WPS :-(");  
    }
  }
}

bool startWPSPBC() {
  Serial.println("WPS config start");
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
      // Well this means not always success :-/ in case of a timeout we have an empty ssid
      String newSSID = WiFi.SSID();
      if(newSSID.length() > 0) {
        // WPSConfig has already connected in STA mode successfully to the new station. 
        Serial.printf("WPS finished. Connected successfull to SSID '%s'\n", newSSID.c_str());
      } else {
        wpsSuccess = false;
      }
  }
  return wpsSuccess; 
}

void loop() {

  if (!sent){
    sendDataToFirebase();
    Serial.println("Sent");
    sent = true;
    }
}

void sendDataToFirebase() {
  
 // WiFiClientSecure client;
  
  String data = "{";
  data = data + "\"to\": \"1001392278284\",";
  data = data + "\"notification\": {";
  data = data + "\"body\": \"asdsadsa\",";
  data = data + "\"title\" : \"Alarm\" ";
  data = data + "} }";
  
  Serial.println("Send data...");
  
  while (!client.connect(fcmServer, 80)) {   // <-- should be 443/80
    Serial.println("connection failed");
    delay(1000);
    //return;  
  }

    Serial.println("Connected to the server..");
    
    client.println("POST /fcm/send HTTP/1.1");
    //client.println("POST / HTTP/1.1");
    client.println("Host: fcm.googleapis.com");
    client.println("Authorization: key=AAAA6Sehiww:APA91bF-22_RnEXyjGgFlD8wnoXSH9PpZGEWjUv1TJ81g8Oh5WSg5SbXtyObJUfEbrxLoVSFmd954GGo7zRx8x6Xm9LZp2tHbs_19mdmeKj2cJgNR4VwLqPKhGzTJpqXWSRw8Lkh-SRT");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    //client.print("Connection: close\r\n\r\n");
    client.println(data.length());
    client.println();
    client.print(data);   
    delay(1000);
    
  if (client.connected()) 
  { 
    client.stop();  // DISCONNECT FROM THE SERVER
  }
  Serial.println();
  Serial.println("closing connection");
  delay(500);

  Serial.println("Data sent...Reading response..");
 /* 
  while (client.available()) {
   char c = client.read();
   Serial.print(c);
  }
  */
  
  Serial.println("Finished!");
  //client.flush();
  //client.stop();
}
