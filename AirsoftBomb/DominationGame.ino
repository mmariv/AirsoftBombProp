void setupDomination(){
  
  loopDomination();
}

void loopDomination(){
  bool defused = false;
  bool resetTimerText = true;
  
  unsigned int startTime = millis();
  unsigned long endTime = startTime + gameTime;

  resetGame();
  while(!defused && gameRunning && millis() < endTime){
    if(resetTimerText){
      lcd.clear();
      lcd.print("Time remaining");  
    }
    showCountDown(gameTime - (millis()-startTime), true);
    resetTimerText = false;

    
  }
}
