/*
 * Name: AirsoftBomb
 * Author: mmariv
 * Created: 07/10/2020
 * Description: Airsoft arduino bomb
*/

#include <Wire.h> 
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>

/* Settings game */
unsigned long gameTime = 1800000;
const int pinLength = 4;

/* Settings general */
const int debounceTime = 50;
const int holdTime = 50;
const char BT_UP = 'a';
const char BT_DOWN = 'b';

const String dominationG = "Domination";
const String defuseG = "Defuse";
const String menuList[]={dominationG,defuseG};
const int menuLength = 2 - 1; //Set depending on menuList array, always reduce by one

/* Global variables */
char cKey; //currentKey
bool gameRunning = false;

/*LCD I2C*/
LiquidCrystal_I2C lcd(0x27, 20, 4);

/*NeoPixel LED*/
const int PixelPin = 11;
const int NumPins = 12;
Adafruit_NeoPixel strip(NumPins, PixelPin, NEO_GRBW + NEO_KHZ800);

/* Colors */
const uint32_t yellow = strip.Color(255, 255, 0);
const uint32_t red = strip.Color(255, 0, 0);
const uint32_t green = strip.Color(0, 255, 0);
const uint32_t blue = strip.Color(0, 0, 255);

/* Keypad */
const byte ROWS = 4;
const byte COLS = 4; 
const char keys[ROWS][COLS] = {
  {'1','2','3','a'},
  {'4','5','6','b'},
  {'7','8','9','c'},
  {'*','0','#','d'}
};

const byte rowPins[ROWS] = {2, 3, 4, 5}; 
const byte colPins[COLS] = {6, 7, 8, 9}; 

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const byte up[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B00000,
  B00000,
};

const byte down[8] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
};

byte bar[8] = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  };

/* Buzzer */
const int buzzerPin = 10;

/* Program */
void setup() {
  defaultTone(200);
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Setup");

  uint32_t yellow = strip.Color(255, 255, 0);
  strip.begin();
  strip.clear();
  strip.setBrightness(32);
  strip.show();
  strip.fill(yellow);
  strip.show();

  lcd.init();                     
  lcd.backlight();
  lcd.setCursor(2,0);

  /* Load screen */
  lcd.setCursor(0, 0);
  lcd.print("Airsoft Bomb");
  lcd.setCursor(0, 1);
  lcd.print("Loading...");

  lcd.createChar(0,up);
  lcd.createChar(1,down);
  lcd.createChar(2, bar);

  keypad.setHoldTime(50);
  keypad.setDebounceTime(50);
  //keypad.addEventListener(keypadEvent);

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  //showMenu();
  setupDefuse();
}

void keypadEvent(KeypadEvent key){
  if(gameRunning == false) return;
  switch (keypad.getKey()){
    case 'c':
      stopGame();
    break;
  }
}

void stopGame(){
  lcd.clear();
  lcd.print("Stop game?");
  lcd.setCursor(0,1);
  lcd.print("Press D");
  unsigned int timeout = 5000;
  unsigned int start = millis();
  while(gameRunning && (millis() < (start + timeout))){
    cKey = keypad.getKey();
    if(cKey == 'd'){
      gameRunning = false;
    }
  }
}

void showMenu(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MAIN MENU");
    lcd.setCursor(0, 1);
    lcd.print("A=UP B=DOWN");
    delay(2000);
    int menuIndex = 0;
    browseMenu(menuIndex);
    while(1){
      browseMenu(menuIndex);
      gameRunning = false;
      cKey = keypad.waitForKey();
      defaultTone(100);
      if(cKey == BT_UP && menuIndex>0){
        menuIndex--;
      }
      if(cKey == BT_DOWN && menuIndex<menuLength){
        menuIndex++;
      }
      
      if(cKey == 'd'){
        startGame(menuIndex);
      }
    }
}

void defaultTone(int duration){
  tone(buzzerPin, 1600, duration);
}

void browseMenu(int index){
  lcd.clear();
  lcd.print(menuList[index]);
  lcd.setCursor(0,1);
  lcd.print("d=ENTER");
  setNavArrows(index, menuLength);
  delay(500);
}

void setNavArrows(byte index, byte maxIndex){

  if(index == 0){
    lcd.setCursor(15,1);
    lcd.write(1); 
  }
  if(index == maxIndex){  
    lcd.setCursor(15,0);
    lcd.write(0);
  }
  if(index > 0 && index<maxIndex){
    lcd.setCursor(15,1);
    lcd.write(1);
    lcd.setCursor(15,0);
    lcd.write(0);  
  }
}

void startGame(int index){
  String menu = menuList[index];
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Starting game:");
  lcd.setCursor(0,1);
  lcd.print(menu);

  if(menu == dominationG){
    setupDomination();
  }
  else if(menu == defuseG){
    setupDefuse();
  }
}

void showTimeout(unsigned long timeout){
  unsigned long start = millis();
  while(millis() < (start + timeout)){
    showCountDown(timeout - 
    (millis()-start), false);
  }
}

void showCountDown(unsigned long remaining, bool showMillis){
  lcd.setCursor(4,1);
  /* minutes */
  if((remaining/60000) < 10){
    lcd.print("0");
    lcd.print(remaining/60000);
  }
  else{
    lcd.print(remaining/60000);
  }
  lcd.print(':');
  if((remaining/1000)%60 < 10){
    lcd.print("0");
    lcd.print((remaining/1000)%60);
  }
  else
  {
    lcd.print((remaining/1000)%60);
  }
  if(showMillis){
    lcd.print(':');
    lcd.print(999-millis()%1000);
  }
}

void startTimeout(){
  lcd.clear();
  lcd.print("Starting game in:");
  showTimeout(10000);
}

void resetGame(){
  gameRunning = true;
}

void cancelGameMenu(unsigned int showForTime){
  lcd.clear();
  lcd.print("Cancel game?");
  lcd.setCursor(0, 1);
  lcd.print("Press D");
  unsigned int endTime = millis() + showForTime;
  while(gameRunning && millis() < endTime){
    cKey = keypad.getKey();
    if(cKey == 'd'){
      gameRunning = false;
    }
    if(cKey == 'c'){
      break;
    }
  }
}
