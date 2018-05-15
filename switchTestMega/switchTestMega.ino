void setup() {
 pinMode(53,INPUT_PULLUP);
 Serial.begin(9600);

}

void loop() {
 
if((digitalRead(53)) == 0)
{
  Serial.println("ok");
}
else
{
  Serial.println("off");
}
}
