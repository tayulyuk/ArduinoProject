// Wi-Fi 모듈을 PubSubClient와 함께 사용하기 위해 선택한 ESP8266용 라이브러리 
#include <WiFiEsp.h> 
#include <WiFiEspClient.h> 

// MQTT 통신을 위한 라이브러리 
#include <PubSubClient.h> 

// DHT22 온습도 센서 라이브러리 
#include <DHT.h> 

// DS3231 RTC 모듈 라이브러리 
#include <DS3231.h> 

// Nokia5110 LCD 사용을 위한 라이브러리 
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h> 

// 네오픽셀 LED 사용을 위한 라이브러리 
#include <Adafruit_NeoPixel.h> 

// 메모리 관리를 위한 F함수 또는 PROGMEM을 사용하기 위한 헤더파일. 아두이노 메가를 사용하는 
// 시점에서 큰 의미는 없어졌음
#include <avr/pgmspace.h> 

// MQTT 서버 주소
IPAddress server(123, 123, 123, 123); 

// 공유기 SSID와 비밀번호 
char ssid[] = "ABCDEFG"; 
char pass[] = "password"; 

// Wi-Fi 모듈 상태를 표시하기 위한 변수 
int status = WL_IDLE_STATUS; 

// WI-Fi 모듈 객체 선언 
WiFiEspClient espClient; 

// Wi-Fi 객체를 파라미터로 MQTT 통신을 위한 clietn 생성
PubSubClient client(espClient);

// DHT22 사용을 위한 데이터 핀 설정과 모델 타입 매크로 선언
#define DHTPIN 5 
#define DHTTYPE DHT22 

// DHT22 사용을 위한 객체 생성
DHT dht(DHTPIN, DHTTYPE); 

// GP2Y1010 측정 핀과 측정을 위한 내부 LED 연결 핀 설정 
int measurePin = A0; 
int ledPower = 6; 

// 먼지 측정을 위해 필요한 각종 변수 선언 (자세한 내용은 타 사이트 참조) 
int samplingTime = 280; 
int deltaTime = 40; 
int sleepTime = 9680; 

float voMeasured = 0; 
float calcVoltage = 0; 
float dustDensity = 0; 

float totSum = 0; 
float totCnt = 1.0;
float prevD = 0.0; 

// DS3231 RTC 모듈 객체 생성 
DS3231 rtc(SDA, SCL); 

// 날짜와 시간 표시를 위한 변수 
char *dateStr; 
char *prevDateStr = ""; 

// Nokia 5110 LCD 사용을 위한 객체 생성 
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 8, 9, 10, 11); 

// 네오픽셀 LED 사용을 위한 핀 번호 및 LED의 index 매크로 선언 
#define PIN 4 
#define LEDNUM 1 

// 네오픽셀 LED 사용을 위한 객체 생성 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDNUM, PIN, NEO_GRB + NEO_KHZ800); 

void setup() { 
  // 시리얼 초기화 
  Serial.begin(9600); 
  
  // Wi-Fi 모듈 사용을 위한 Serial1 초기화 
  Serial1.begin(9600); 
  WiFi.init(&Serial1); 
  
  // Wi-Fi 모듈 연결 여부 확인 
  if (WiFi.status() == WL_NO_SHIELD) { while (true); } 
  
  // AP에 접속 
  while ( status != WL_CONNECTED) { status = WiFi.begin(ssid, pass); } 
  
  // MQTT를 위한 서버 정보 설정 및 콜백함수 설정. 나는 MQTT publishing만 할 것이기에
  // 콜백 함수는 빈 함수로 남겨둠
  client.setServer(server, 1883); 
  client.setCallback(callback); 
  
  // 기타 설정들은 다름 함수로 뺌 
  otherSetting(); 
  delay(2000);
  }
  
  void otherSetting() { 
    // DS3231 RTC 모듈 초기화 
    rtc.begin(); 
    // 시작 날짜와 시간을 설정하는 코드로 최초 사용시에만 필요함
    // rtc.setDate(25, 1, 2018); 
    // rtc.setTime(23, 21, 45);
    // rtc.setDOW(4); 
    
    // 네오픽셀 LED 초기화 
    strip.begin(); 
    pinMode(ledPower,OUTPUT); 
    
    // Nokia 5110 LCD 초기화 
    display.begin(); 
    display.setContrast(65); 
    display.setTextSize(1); 
    display.setTextColor(BLACK); 
    
    // DHT22 초기화 
    dht.begin(); 
    } 
    
    void callback(char* topic, byte* payload, unsigned int length) { 
      //MQTT 통신에서 subscrib시 필요한 내용이나 나의 경우 publish만 하므로 그냥 비워둠
      } 
      
      void loop() { 
        // 메인 루프에서는 MQTT 통신이 끊어져있으면 연결을 하고 연결되어있으면 데이터를 모아서 
        // MQTT 서버로 전송하는 작업을 진행함 
        if (!client.connected()) { 
          // 연결이 끊어져있으면 다시 연결 시도  
          reconnect();
          } else { 
            // MQTT 서버에 연결되어있으면 데이터를 수집하여 MQTT 서버로 보냄 
            getValueAndSendData(); 
            // 위 작업을 5초마다 한 번씩 수행
            delay(5000);
            } 
    } 
            
    void getValueAndSendData() {
              // 온도, 습도측정 값을 담을 변수를 선언하고 DHT22 센서로부터 측정값을 받아옴
              float temperature = dht.readTemperature(); 
              float humidity = dht.readHumidity(); 
              // 먼지 센서의 경우 조금 복잡한 계산이 필요하여 따로 함수 처리를 함
              float d = getdust(); 
              // 간혹 먼지 센서에서 노이즈값이 나오므로 이에 대한 처리를 해줌(값자기 100 가까운 값이
              // 나오기도 하나 음수에 대해서만 처리했음 
              if (d < 0.0) { d = prevD; } prevD = d; 
              // DS3231로부터 요일값을 받아옴 
              dateStr = rtc.getDOWStr(FORMAT_SHORT); 
              // 현재 먼지 센서의 측정 값을 순간값이 아닌 그 시점까지의 그날 평균으로 보여주고 있으며(1일 평균)
              // 날짜가 바뀌면 다시 처음부터 계산하도록 처리함 
              if (prevDateStr == dateStr) { 
                totSum += d; totCnt += 1.0; 
              } else { 
                totSum = d; totCnt = 1.0; 
                }
                prevDateStr = dateStr; 
                float avgD = totSum / totCnt; 
                // 먼지 측정 값에 따라 네오픽셀 LED의 색상을 변경함 
                if (avgD <= 30) { 
                  strip.setPixelColor(0, 0, 0, 255); 
                  } else if (avgD <= 80) {
                    strip.setPixelColor(0, 0, 255, 0);
                    } else if (avgD <= 150) { 
                      strip.setPixelColor(0, 255, 255, 0);
                      } else if (avgD >= 151) { 
                        strip.setPixelColor(0, 255, 0, 0); 
                        } strip.show(); 
                        // 센서 데이터들을 JSON 포맷으로 만들어 MQTT 서버로 전송함
                        // 이 JSON 포맷은 시계열 데이터용 NoSQL인 openTSDB에 넣기 위한 포맷임
                        // 주의할 것은 PubSubClient 라이브러리는 전송 가능한 패킷 사이즈가 128byte로 
                        // 기본 설정되어 더 큰 크기의 패킷 전송을 위해서는 MQTT_MAX_PACKET_SIZE 값을 
                        // 변경해주어야 한다. 나는 넉넉하게 1024로 설정했다. 
                        char attributes[393]; 
                        String dtStr = String(rtc.getUnixTime(rtc.getTime())); 
                        String payload = F("["); 
                        payload += F("{"); 
                        payload += F("\"type\":\"Metric\""); 
                        payload += F(",");
                        payload += F("\"metric\":\"mqtt.home.pcroom\""); 
                        payload += F(","); 
                        payload += F("\"timestamp\":"); 
                        payload += dtStr; 
                        payload += F(","); 
                        payload += F("\"value\":"); 
                        payload += String(temperature); 
                        payload += F(","); 
                        payload += F("\"tags\": {");
                        payload += F("\"type\":\"temperature\","); 
                        payload += F("\"loc\":\"pcroom\""); 
                        payload += F("}},"); 
                        // payload.toCharArray( attributes, 131 ); 
                        // client.publish("/mqtt", attributes); 
                        //Serial.println(attributes); 
                        payload += F("{");
                        payload += F("\"type\":\"Metric\""); 
                        payload += F(","); 
                        payload += F("\"metric\":\"mqtt.home.pcroom\""); 
                        payload += F(","); 
                        payload += F("\"timestamp\":"); 
                        payload += dtStr; 
                        payload += F(","); 
                        payload += F("\"value\":"); 
                        payload += String(humidity); 
                        payload += F(","); 
                        payload += F("\"tags\": {"); 
                        payload += F("\"type\":\"humidity\","); 
                        payload += F("\"loc\":\"pcroom\"");
                        payload += F("}},"); 
                        // payload.toCharArray( attributes, 131 ); 
                        // client.publish("/mqtt", attributes); 
                        //Serial.println(attributes); 
                        payload += F("{"); 
                        payload += F("\"type\":\"Metric\""); 
                        payload += F(","); 
                        payload += F("\"metric\":\"mqtt.home.pcroom\""); 
                        payload += F(","); 
                        payload += F("\"timestamp\":"); 
                        payload += dtStr; 
                        payload += F(","); 
                        payload += F("\"value\":"); 
                        payload += String(avgD); 
                        payload += F(","); 
                        payload += F("\"tags\": {"); 
                        payload += F("\"type\":\"dust\","); 
                        payload += F("\"loc\":\"pcroom\""); 
                        payload += F("}}"); 
                        payload += F("]"); 
                        // String 타입으로 만든 JSON 문자열을 byte 배열로 변경한 후 서버로 publishing
// "/mqtt"는 토픽 이름 
payload.toCharArray( attributes, 393 ); 
client.publish("/mqtt", attributes);
//Serial.println(attributes); 
// 날짜, 시간, 요일, 온도, 습도, 먼지 등의 값을 LCD에 출력 
display.clearDisplay(); 
display.setCursor(0,0); 
display.print(rtc.getTimeStr(FORMAT_SHORT)); 
display.print(F(" [")); 
display.print(dateStr); 
display.print(F("]\n")); 
display.println(rtc.getDateStr(FORMAT_LONG, FORMAT_BIGENDIAN, '-')); 
display.print(F("\nT: ")); 
display.println(temperature, 1); 
display.print(F("H: ")); 
display.println(humidity, 1); 
display.print(F("D: ")); 
display.print(avgD, 1); 
display.display(); 
} 

// MQTT 서버에 연결이 안되었을 시 재연결하는 함수 
void reconnect() { 
  while (!client.connected()) { 
    // 아래 connect 함수를 통해 전달하는 3개의 문자열은 그냥 임의로 정해도 됨 
    if (client.connect("ARDUINO", "mazdah", "abcdefg")) { 
      Serial.println(F("connected")); 
      } else { 
        Serial.print(F("failed")); delay(5000); 
        }
    }
} 
        // 먼지 측정을 위한 함수 
        // 고수들은 한번 측정된 값을 사용하지 않고 짧은 시간에 여러번 측정을 하여 그 평균을 사용함
 float getdust(){
    digitalWrite(ledPower,LOW);
    delayMicroseconds(samplingTime);
    voMeasured = analogRead(measurePin);
    
    delayMicroseconds(deltaTime); 
    digitalWrite(ledPower,HIGH); 
    delayMicroseconds(sleepTime); 
    
    calcVoltage = voMeasured * (5.0 / 1024); 
    dustDensity = (0.17 * calcVoltage - 0.1) * 1000.0;
    return(dustDensity); 
 }



