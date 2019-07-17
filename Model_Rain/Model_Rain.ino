
int Raindrops_pin = A0;  // 빗방울센서 핀을 A0으로 설정
//int LED = 7;           // 노란색 LED 핀을 7번핀으로 설정
//int Buzzer = 13;         // Buzzer 핀을 13번핀으로 설정
 
void setup(){
  Serial.begin(9600);   // Serial monitor 전원 인가
  pinMode(A0 , INPUT);   // 빗방울 감지핀을 아날로그 입력으로 설정
 
}
 
void loop(){
 
  Serial.println(analogRead(A0));  // 센서값을 시리얼모니터로 전송
  delay(100);                       // 센서값 간격 0.1초로 설정
  if(analogRead(A0) < 500){         // 빗방울 감지핀의 수위가 500미만이면  비 옴
    Serial.println("Ok Raining");
  }                                  
 else {                              // 빗방울 감지핀의 출력값이 500이상이면 비 안옴  
    Serial.println("Stop Rain");
 } 
}
