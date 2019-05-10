/*
 * W5100/ENC28J60 Ethernet Module MQTT Publish.
 */

#include <UIPEthernet.h> // https://github.com/UIPEthernet/UIPEthernet
//#include <SPI.h>
//#include <Ethernet.h>
#include <PubSubClient.h> // https://github.com/knolleary/client

#define INTERVAL        10 // 10秒毎にPublish
#define MQTT_SERVER     "119.205.235.214"  // You need change
//#define MQTT_SERVER     "broker.hivemq.com"
//#define MQTT_SERVER     "iot.eclipse.org"
#define MQTT_PORT       1883
#define MQTT_TOPIC      "Arduino/UNO"
#define MQTT_WILL_MSG   "I am leaving..." // You can change
#define RUNNING_LED     13 // 0: Disable RUNNING_LED

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(MQTT_SERVER ,MQTT_PORT,callback, ethClient);

unsigned long nextMillis;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void errorDisplay(char* buff) {
  int stat = 0;
  Serial.print("Error:");
  Serial.println(buff);
  while(1) {
    digitalWrite(RUNNING_LED,stat);
    stat = !stat;
    delay(100);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.print("Ethernet begin....");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("faild");
   // while(1) {}
   delay(500);
  }
  Serial.println("ok");
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
  
 // pubsubClient.setServer(MQTT_SERVER, MQTT_PORT);
 // pubsubClient.setCallback(callback);

  char clientid[30];
  IPAddress ip = Ethernet.localIP();
  //sprintf(clientid,"%03d-%03d-%03d-%03d",ip[0], ip[1], ip[2], ip[3]);
  sprintf(clientid,"Arduino-%03d",(int)ip[3]);
  Serial.print("clientid=");
  Serial.println(clientid);
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //if (pubsubClient.connect(clientid,MQTT_TOPIC,0,0,MQTT_WILL_MSG)) {
    if (client.connect(clientid) ){
      Serial.println("connected");
      client.publish(MQTT_TOPIC ,"okkkk");
      client.subscribe(MQTT_TOPIC,1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  nextMillis = millis();
}

void loop() {
  static int value = 0;
  char msg[50];
  unsigned long now;

  client.loop();
  now = millis();
  if ( long(now - nextMillis) > 0) {
    nextMillis = millis() + (INTERVAL * 1000);
    ++value;
    snprintf (msg, 75, "MQTT from Arduino/UNO %06d",value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    if (!client.publish(MQTT_TOPIC, msg) ) {
       errorDisplay("Failed to Publsh");
    }
  }
}
