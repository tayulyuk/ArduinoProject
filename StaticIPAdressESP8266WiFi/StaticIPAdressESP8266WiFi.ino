#include <ESP8266WiFi.h>
IPAddress ip(10, 0, 1, 15);
IPAddress gateway(10, 0, 2, 200);
IPAddress subnet(255, 0, 0, 0);
IPAddress DNS(10, 0, 2, 1);
const char* ssid     = "XXXXXXX";
const char* password = "YYYYYYY";

void wifi_init(){
  WiFi.config(ip, gateway, subnet, DNS);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(200);
  }
  Serial.println();
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Fail connecting");
    delay(5000);
    ESP.restart();
  }
  Serial.print("   OK  ");
  Serial.print("Module IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  wifi_init();
}

void loop() {
  // put your main code here, to run repeatedly:

}
