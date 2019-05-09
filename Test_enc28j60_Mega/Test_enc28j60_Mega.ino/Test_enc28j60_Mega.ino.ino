#include <UIPEthernet.h>
#include <PubSubClient.h>

int led = 13;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 1, 15 };
byte ip[]     = { 192, 168, 1, 17 };

boolean blink = false;

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
 // Copie du payload dans byte* pl pour usage dans les fonctions, car il
 // est vide lors d'un appel a publish
 byte* pl = (byte*)malloc(length);
 memcpy(pl,payload,length);

 // Test du TOPIC
 if (strcmp(topic,"input/set")==0) {
 blink = !blink;
 if(blink) {
 digitalWrite(led, HIGH);
 } else {
 digitalWrite(led, LOW);
 }
 client.publish("output/system/","set");
 }

 // Test du PAYLOAD
 if (memcmp(pl, "comtest",length)==0) {
 blink = !blink;
 if(blink) {
 digitalWrite(led, HIGH);
 } else {
 digitalWrite(led, LOW);
 }
 }
 if (memcmp(pl, "ping",4)==0) {
 client.publish("output/system/","ok");
 }

 free(pl);
}

void setup()
{
 pinMode(led, OUTPUT);
 digitalWrite(led, HIGH);
 Ethernet.begin(mac, ip);
 if (client.connect("arduinoClient")) {
 client.publish("output/system/","ready");
 client.subscribe("input/#");
 }
 digitalWrite(led, LOW);
}

void loop()
{
 client.loop();
}

