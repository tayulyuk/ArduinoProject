String command;

void setup(void)
{  
    Serial.begin(115200);   

    //pin settings
    AllSetOutPutMode();  
    //일단 HIGH 모드로 모두 끄고 시작한다.
    AllLOWSetDigitalWrite();
}

void loop() {
  if(Serial.available() ) 
  {
    char c = Serial.read();
    if(c == '\n')
    {
      parseCommand(command);
      command = "";      
    }
    else
    {
      command += c;
    }
  }  
}

//프로토콜 pinOn,pinIdle,pinOff     (명령 + " " + 핀번호)
void parseCommand(String com)
{
  String part1;
  String part2;  
  part1 = com.substring(0,com.indexOf(" "));
  part2 = com.substring(com.indexOf(" ") + 1);
  int pin = changeNum(part2.toInt());
  
  if(part1.equalsIgnoreCase("pinOn"))
  {   
    digitalWrite(pin, LOW);  //+
    //환풍기 번호(34,35)가 아니면
    if(pin != 34 && pin != 35)
      digitalWrite(pin + 1, HIGH); //- 
  }
  else if(part1.equalsIgnoreCase("pinIdle") && pin != 34 && pin != 35)
  {
     //환풍기 번호(34,35)가 아니면 // 오더가 있을수 없겠지만 혹시나 예외 처리 합니다.
    if(pin != 34 && pin != 35){  
      digitalWrite(pin, LOW);  //+
      digitalWrite(pin + 1, LOW);  //+   
    }
  }
  else if(part1.equalsIgnoreCase("pinOff"))
  {   
    digitalWrite(pin, HIGH);//-  
     //환풍기 번호(34,35)가 아니면
    if(pin != 34 && pin != 35)
      digitalWrite(pin +1 , LOW);//+  
  }  
   else if(part1.equalsIgnoreCase("ping"))
   {
    Serial.println("pong"); //클라이언트에게 다시 보낸다.
   }
  else{
    Serial.println("COMMAND NOT RECONGNIZED");
  }

  Serial.println(com); //다시 클라이언트로 보내는 값.
}

//하우스 번호(클라이언트)에 마췄다.
int changeNum(int orderNum)
{
  int order;
  if(orderNum == 1)
    order = 2;
    if(orderNum == 2)
    order = 4;
    if(orderNum == 3 )
    order = 6;
    if(orderNum == 4 )
    order = 8;
    if(orderNum == 5 )
    order = 10;
    if(orderNum == 6 )
    order = 12;
    
    if(orderNum == 7 )
    order = 22;
    if(orderNum == 8 )
    order = 24;
    if(orderNum == 9)
    order = 26;
    if(orderNum == 10 )
    order = 28;
    if(orderNum == 11 )
    order = 30;
    if(orderNum == 12 )
    order = 32;
    
    //환풍기.앞
    if(orderNum == 13 )
    order = 34;
    //환풍기.뒤
    if(orderNum == 14 )
    order = 35;
    return order;
}

void AllSetOutPutMode()
{
  /*
  //1번
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  //2번
  pinMode(4, OUTPUT);  
  pinMode(5, OUTPUT);
   //3번
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
   //4번
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  //5번
  pinMode(10, OUTPUT); 
  pinMode(11, OUTPUT);
  //6번
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  ==  14~ 21
   //7번
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  //8번
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
   //9번
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  //10번
  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
   //11번
  pinMode(30, OUTPUT);  
  pinMode(31, OUTPUT);
   //12번
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);  

   //13번 환풍기.앞
  pinMode(34, OUTPUT);
  //14번 환풍기.뒤
  pinMode(35, OUTPUT);  
  */
  for(int i = 2; i <= 35 ; i++)
  {
    if( 13 < i && i < 22 )
          continue;
    else    
      pinMode(i, OUTPUT);       
  }
}

void AllLOWSetDigitalWrite()
{
/*
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  
  digitalWrite(22, LOW);
  digitalWrite(23, LOW);
  digitalWrite(24, LOW);
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  digitalWrite(28, LOW);
  digitalWrite(29, LOW);
  digitalWrite(30, LOW);
  digitalWrite(31, LOW);
  digitalWrite(32, LOW);
  digitalWrite(33, LOW);  
  //환풍기 앞
  digitalWrite(34, LOW);
  //환풍기 뒤
  digitalWrite(35, LOW);  
*/
/////////
  for(int i = 2; i <= 35 ; i++)
  {
    if( 13 < i && i < 22 )
          continue;
    else    
       digitalWrite(i, HIGH);       
  }
}


