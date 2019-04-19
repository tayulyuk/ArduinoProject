int cds = A1;
int LED = 8;
 
void setup() {
  Serial.begin(9600);
  pinMode(cds,INPUT);
  pinMode(LED,OUTPUT);
}
 
void loop() {
  int val = analogRead(cds);
  Serial.print("cds_sensor: ");
  Serial.println(val);
 
  if(val>900){
    digitalWrite(LED,HIGH);
    Serial.println("LED ON");
  }
  else{
    digitalWrite(LED,LOW);
    Serial.println("LED OFF");
  }
  delay(200);
}
