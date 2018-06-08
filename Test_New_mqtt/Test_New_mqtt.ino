#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>

#include <PubSubClient.h>
#include <SPI.h>

IPAddress server(14,63,171,33);
char ssid[] = "KT_GiGA_2G_76C7";           // your network SSID (name)
char pass[] = "4jf38gf684";           // your network password
int status = WL_IDLE_STATUS; // the Wifi radio's status

// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

//SoftwareSerial soft(2,3); // RX, TX //not used
void setup() {
// initialize serial for debugging
Serial.begin(9600);
// initialize serial for ESP module
Serial1.begin(9600);
// initialize ESP module
WiFi.init(&Serial1);

// check for the presence of the shield
if (WiFi.status() == WL_NO_SHIELD) {
Serial.println("WiFi shield not present");
// don't continue
while (true);
}

// attempt to connect to WiFi network
while ( status != WL_CONNECTED) {
Serial.print("Attempting to connect to WPA SSID: ");
Serial.println(ssid);
// Connect to WPA/WPA2 network
status = WiFi.begin(ssid, pass);
}

// you're connected now, so print out the data
Serial.println("You're connected to the network");

//connect to MQTT server
client.setServer(server, 1883);
client.setCallback(callback);
}

//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);

Serial.print("] ");
for (int i=0;i<length;i++) {
char receivedChar = (char)payload;
Serial.print(receivedChar);
if (receivedChar == '0')
Serial.println("Off");
if (receivedChar == '1')
Serial.println("On");

}
Serial.println();
}

void loop() {
// put your main code here, to run repeatedly:
if (!client.connected()) {
reconnect();
}
client.loop();
}

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("Attempting connection…");
// Attempt to connect, just a name to identify the client
if (client.connect("AMega","xxxxxx","xxxxxxxxxxx")) {
Serial.println("connected");
// Once connected, publish an announcement…
// client.publish("command","Hello World");
// … and resubscribe
client.subscribe("switch",0);

} else {
Serial.print("failed");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");
// Wait 5 seconds before retrying
delay(5000);
}
}
}
