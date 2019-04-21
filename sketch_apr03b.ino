#include <LiquidCrystal.h>
#include <Adafruit_RGBLCDShield.h>
#include <Time.h>
#include <TimeLib.h>

#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

unsigned long timer1 = 0, timer2 = 0, timer3 = 0;
int Happiness, Fullness;

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

LiquidCrystal lcdnew(7,9,10,11,12,13);

const int buttonPin = 2;
const int ledPin =  13;

int buttonState = 0;

byte hourGlass1[8] = {
  B11111,
  B11111,
  B01110,
  B00100,
  B00100,
  B01010,
  B10001,
  B11111
};
byte hourGlass2[8] = {
  B11111,
  B10001,
  B01010,
  B00100,
  B00100,
  B01110,
  B11111,
  B11111
};
byte egg[8] = {
  B00100,
  B01010,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B01110
};
byte halfBrokenEgg[8] = {
  B00000,
  B00000,
  B00000,
  B10001,
  B11011,
  B10101,
  B10001,
  B01110
};
byte otherHalfBrokenEgg[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B10001,
  B10101,
  B01110
};
byte animal[8] = {
  B00011,
  B00011,
  B00010,
  B01111,
  B10010,
  B10011,
  B01110,
  B01010
};
byte chickBody[8] = {
  B00000,
  B00100,
  B01010,
  B10101,
  B10001,
  B10001,
  B01110,
  B01010
};
byte chickHappyR[8] = {
  B00000,
  B00000,
  B00000,
  B10000,
  B01000,
  B00100,
  B00000,
  B00000
};
byte chickHappyL[8] = {
  B00000,
  B00000,
  B00000,
  B00001,
  B00010,
  B00100,
  B00000,
  B00000
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.print("Menu");
  lcd.setBacklight(TEAL);
  lcd.createChar(15, hourGlass1);
  lcd.createChar(14, hourGlass2);
  lcd.createChar(13, egg);
  lcd.createChar(12, halfBrokenEgg);
  lcd.createChar(11, otherHalfBrokenEgg);  
  lcd.createChar(10, animal);
  lcd.createChar(9, chickBody);
  lcd.createChar(8, chickHappyR);
  lcd.createChar(7, chickHappyL);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void LoadingIcon(){
  delay(500);
  lcd.setCursor(15,1);
  lcd.write(byte(15));
  delay(500);
  lcd.setCursor(15,1);
  lcd.write(byte(14));
  delay(500);
  lcd.setCursor(15,1);
  lcd.write(byte(15));
  delay(500);
  lcd.setCursor(15,1);
  lcd.write(byte(14)); 
}

void ChickenRunning(){
    lcd.write(byte (10));
    delay(500);
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.write(byte (10));
    delay(500);
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.write(byte (10));
    delay(500);
    lcd.clear();
    lcd.setCursor(9,0);
    lcd.write(byte (10));
    delay(500);
    lcd.clear();
    lcd.setCursor(12,0);
    lcd.write(byte (10));
    delay(500);
    lcd.clear();
    lcd.setCursor(15,0);
    lcd.write(byte (10));
    delay(500);
    lcd.clear();
}

bool CheckTime(int timeChange){
  ChickenRunning();
  lcd.setCursor(0,0);
  lcd.write(byte(13));
  lcd.print("<-Your pet");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(byte(13));
  timer1 = millis();
    while (true){
      if (millis() - timer1 >= timeChange){ 
        break;
      }
  }
  return true;
}

void HatchEgg(){
  CheckTime(5000);
    if (CheckTime){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write(byte(12));
      lcd.write(byte(11));
      lcd.print("Your Egg ");
      lcd.setCursor(0,1);
      lcd.print("has hatched!");
    }
}

void YoungStage(){
  Happiness = 2;
  uint8_t buttons = lcd.readButtons();
  Fullness = 2;
  unsigned long Age = timer1;
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(Age/1000);
  }

void HappyChick(){
  lcd.setCursor(0,0);
  lcd.write(byte(7));
  lcd.write(byte(9));
  lcd.write(byte(8));
  delay(500);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.write(byte(9));
  delay(500);
  lcd.setCursor(0,0);
  lcd.write(byte(7));
  lcd.write(byte(9));
  lcd.write(byte(8));
  delay(500);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.write(byte(9));
}

void Feed(int Full, int Happy){
  if (Fullness < 4 & Happiness <= 2){
    Fullness +=1;
    if (Happiness<2){
      Happiness+=1;
      lcd.print("Chick fed!");
    }
  }
}

void StartNewPet(){
    lcd.setBacklight(GREEN);
    lcd.print("Laying your egg");
    LoadingIcon();
    delay(1000);
    lcd.clear();
    HatchEgg();
    delay(2000);
    
    }

void LoadPet(){
  lcd.setBacklight(VIOLET);
  lcd.print("Load Last Pet");
  LoadingIcon();
}

void DeleteLastPet(){
  lcd.setBacklight(YELLOW);   
  lcd.print("Deleting last");
  lcd.setCursor(0,1);
  lcd.print("pet...");
  LoadingIcon();
}   

void loop() {
  uint8_t buttons = lcd.readButtons();
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  }
  else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
  if (buttons & BUTTON_RIGHT){
    lcd.clear();
    StartNewPet();
    YoungStage();
    if (buttons & BUTTON_RIGHT){
      Feed(Fullness, Happiness); 
      lcd.print("LMAO");
      HappyChick();
    }
  }

  else if (buttons & BUTTON_DOWN){
    lcd.clear();
    LoadPet();
  }
  
  else if (buttons & BUTTON_UP){
    lcd.clear();
    DeleteLastPet();
  }
}
