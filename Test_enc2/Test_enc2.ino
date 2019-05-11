/*
 Basic MQTT example with Authentication
 
  - connects to an MQTT server, providing username
    and password
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <SPI.h>
#include <UIPEthernet.h>
#include "PubSubClient.h"

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 119,205,235,214  };
byte ip[]     = { 192, 168, 0, 100 };

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip,52);
  if (client.connect("arduinoClient", "testuser", "testpass")) {
    Serial.println("not  connected");
    client.publish("outTopic","hello world");
    client.subscribe("inTopic");
  }
  Serial.println("yes  connected");
}

void loop()
{
  client.loop();
}
