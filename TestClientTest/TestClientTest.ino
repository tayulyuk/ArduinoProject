#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>

char ssid[] = "KT_GiGA_2G_76C7";       //  your network SSID (name) 
char pass[] = "4jf38gf684";   // your network password

int status = WL_IDLE_STATUS;
IPAddress server(14,63,171,33);  // Google

// Initialize the client library
WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  delay(1000);
  if ( status != WL_CONNECTED) { 
    Serial.println(status);
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while(true);
  } 
  else {
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {

}
