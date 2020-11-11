const String pinChars = {"0123456789"};
const unsigned int showPinMenuTimeout = 5000;

void setupDefuse(){
  char pin[pinLength];
  char pinCheck[pinLength];
  int pinCount = 0;
  lcd.clear();
  lcd.print("Enter pin");
  lcd.setCursor(0,1);
  lcd.print("PIN=");
  while(pinCount < pinLength){
    cKey = getPinNumber();
    if(cKey != 'x'){
      pin[pinCount] = cKey;
      pinCount++;
      lcd.print(cKey);
    }
  }
  delay(1000);
  lcd.clear();
  lcd.print("Confirm pin");
  lcd.setCursor(0,1);
  lcd.print("PIN=");
  pinCount = 0;
  while(pinCount < pinLength){
    cKey = getPinNumber();
    if(cKey != 'x'){
      pinCheck[pinCount] = cKey;
      pinCount++;
      lcd.print(cKey);
    }
  }
  delay(1000);
  if(pinsEqual(pin, pinCheck) == false){
    lcd.clear();
    lcd.print("Pin codes not");
    lcd.setCursor(0,1);
    lcd.print("matching!");
    delay(1000);
    return;
  }
  loopDefuse(pin);
}

char getPinNumber(){
  cKey = keypad.getKey();
  if(cKey && pinChars.indexOf(cKey) > -1){
    return cKey;
  }
  return 'x';
}

bool pinsEqual(char * pin1, char * pin2){
  Serial.println(pin1);
  Serial.println(pin2);
  for(int i = 0; i<pinLength;i++){
    if(pin1[i] != pin2[i]) return false;
  }
  return true;
}

void loopDefuse(char * pinCheck){
  startTimeout();

  bool defused = false;
  char pin[pinLength];
  int pinCount = 0;

  unsigned int startTime = millis();
  unsigned long endTime = startTime + gameTime;
  
  resetGame();
  while(!defused && gameRunning && millis() < endTime){
    lcd.clear();
    lcd.print("Time remaining");
    showCountDown(gameTime - (millis()-startTime), true);

    cKey = keypad.getKey();
    if(cKey && pinChars.indexOf(cKey) > -1){
      lcd.clear();
      lcd.print("PIN:");
      lcd.setCursor(0,1);
      pinCount = 0;
      pin[pinCount] = cKey;
      lcd.print(cKey);
      unsigned int pinMenuEnd = millis() + showPinMenuTimeout;
      while(pinCount < pinLength && millis() < pinMenuEnd){
        cKey = keypad.getKey();
        if(cKey && pinChars.indexOf(cKey) > -1){
          lcd.print(cKey);
          pin[pinCount] = cKey;
        }
        
      }
    }
    
  }
}
