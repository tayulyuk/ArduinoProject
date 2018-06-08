// Wi-Fi 모듈을 PubSubClient와 함께 사용하기 위해 선택한 ESP8266용 라이브러리 
#include <WiFiEsp.h> 
#include <WiFiEspClient.h> 

// MQTT 통신을 위한 라이브러리 
#include <PubSubClient.h> 

// DHT22 온습도 센서 라이브러리 
#include <DHT.h> 

// MQTT 서버 주소
IPAddress server(14, 63, 171, 33); 

// 공유기 SSID와 비밀번호 
char ssid[] = "KT_GiGA_2G_76C7"; 
char pass[] = "4jf38gf684"; 

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


// 날짜와 시간 표시를 위한 변수 
char *dateStr; 
char *prevDateStr = ""; 


// 네오픽셀 LED 사용을 위한 핀 번호 및 LED의 index 매크로 선언 
#define PIN 4 
#define LEDNUM 1 


void setup() { 
  // 시리얼 초기화 
  Serial.begin(9600); 
  
  // Wi-Fi 모듈 사용을 위한 Serial1 초기화 
  Serial1.begin(9600); 
  WiFi.init(&Serial1); 
  
  // Wi-Fi 모듈 연결 여부 확인 
  if (WiFi.status() == WL_NO_SHIELD) {  while (true); } 
  
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
 
    // 시작 날짜와 시간을 설정하는 코드로 최초 사용시에만 필요함
    // rtc.setDate(25, 1, 2018); 
    // rtc.setTime(23, 21, 45);
    // rtc.setDOW(4); 
    
    // 네오픽셀 LED 초기화 
  
    pinMode(ledPower,OUTPUT); 
    
  
 
    
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



