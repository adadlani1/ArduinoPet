#include <LiquidCrystal.h>
#include <Adafruit_RGBLCDShield.h>
#include <Time.h>
#include <TimeLib.h>
#include <EEPROM.h>

#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

#define IDLE 1
#define StartupSequence 2
#define NEW  3
#define LOAD 4
#define FEED 5
#define PLAY 6
#define GROW 7
#define MENU 8
#define DELETE 9
#define SAVE 10
#define SAVEDPET 11
#define SIMOVER 12

//list of all of the global variables

int Happiness;
int Fullness;
int developmentStage;

unsigned long timer = 0;
int interval = 1;
unsigned long currentTime = 0;
unsigned long minutes = 0;
boolean limiter = false;
int state;
unsigned long anotherTimer = 0;

String whatIsUserDoingToPet = "";
String MenuThing = "";

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

LiquidCrystal lcdnew(7, 9, 10, 11, 12, 13);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.setBacklight(TEAL);
  state = StartupSequence; 
}

void loop() {
  //buttons are read
  uint8_t buttons1 = lcd.readButtons();
  //switch case for all of the states the pet can have
  switch (state) {
    case IDLE:
      timeShow();
      idleState();
      break;
    case FEED:
      Feed();
      break;
    case PLAY:
      PlayWithPet();
      break;
    //this is the state the pet is in at the start.
    case StartupSequence:
      StartupSequences();
      break;
    case LOAD:
      LoadPet();
      break;
    case NEW:
      newPet();
      break;
    case GROW:
      GrowPet();
      break;
    case MENU:
      Menu();
      break;
    case DELETE:
      DeletePet();
      break;
    case SAVE:
      SavePet();
    case SAVEDPET:
      lcd.setBacklight(BLUE);
      lcd.setCursor(0,0);
      lcd.print("Pet saved. Press");
      lcd.setCursor(0,1);
      lcd.print("reset button.");
      break;
    case SIMOVER:
      lcd.setBacklight(GREEN);
      lcd.setCursor(0,0);
      lcd.print("Pet has gone");
      lcd.setCursor(0,1);
      lcd.print("to sleep");
      break;
  }
}

//function that continuously updates the timer
void timeShow() {
  if (((millis() / 1000) >= (currentTime + 1))) {
    timer++;
    //divides the time into seconds and minutes
    //makes it easier when you are saving as the largest number you 
    //can put into eeprom is 255
    if (timer % 60 == 0) {
      timer = 0;
      minutes++;
    }
    lcd.setCursor(0, 1);
    lcd.print(minutes);
    lcd.print("m");
    lcd.print(timer);
    lcd.print("s");
    currentTime = millis() / 1000;
  }
}

//this is the idle state
void idleState() {
  int fullTime = timer + (minutes * 60);
  uint8_t buttons = lcd.readButtons();
  //creates basic layout on lcd
  lcd.setCursor(0, 0);
  lcd.print("D");
  lcd.print(developmentStage);
  lcd.print(" H");
  lcd.print(Happiness);
  lcd.print(" F");
  lcd.print(Fullness);
  lcd.setCursor(11,0);
  lcd.print("Grow");
  lcd.setCursor(6, 1);
  lcd.print("Feed");
  lcd.setCursor(11,1);
  lcd.print("Play");
  //this is the timer that tracks the fullness and reduces it every a set number of seconds
  if (((millis() / 1000) >= (anotherTimer + 1))) {
    ReduceFullness();
    anotherTimer = millis() / 1000;
  }
  //sets the colour depending on the happiness and fullness
  if(Happiness == 0 || Fullness == 0){
    lcd.setBacklight(YELLOW);
  } else {
    lcd.setBacklight(TEAL);
  }
  ReduceHappiness();
  //if the age is greater than 599 seconds, it stops the simulation and changes state
  if(fullTime > 599){
    state = SIMOVER;
    lcd.clear();
    }
  //limiter stops the development stage from going back to 1 when the seconds is equal to 5
  if (timer == 5 && limiter == false) {
    developmentStage = 1;
    lcd.setCursor(0, 0);
    lcd.print("D");
    lcd.print(developmentStage);
    limiter = true;
  }
  //this is the code for the cursor moving around.
  if (buttons & BUTTON_LEFT && developmentStage >= 1) {
    lcd.setCursor(10, 1);
    lcd.print(" ");
    lcd.setCursor(5, 1);
    lcd.print(">");
    lcd.setCursor(10,0);
      lcd.print(" ");
    whatIsUserDoingToPet = "Feed";

  }
  if ((buttons & BUTTON_RIGHT) && developmentStage >= 1) {
    lcd.setCursor(10, 1);
    lcd.print(">");
    lcd.setCursor(5, 1);
    lcd.print(" ");
    lcd.setCursor(10,0);
      lcd.print(" ");
    whatIsUserDoingToPet = "P";
    MenuThing = "Y";
  }
  if (buttons & BUTTON_UP) {
    if(MenuThing == "Y"){
    lcd.setCursor(10,0);
    lcd.print(">");
    lcd.setCursor(10, 1);
    lcd.print(" ");
    lcd.setCursor(5, 1);
    lcd.print(" ");
      whatIsUserDoingToPet = "G";
    }
  }
  if(buttons & BUTTON_SELECT){
    if(whatIsUserDoingToPet == "Feed"){
      state = FEED;
    } else if(whatIsUserDoingToPet == "P"){
      state = PLAY;
    } else if (whatIsUserDoingToPet == "G"){
      state = GROW;
    }
  }
  if(buttons & BUTTON_DOWN){
    if(whatIsUserDoingToPet == "G"){
      lcd.setCursor(10,0);
      lcd.print(" ");
      lcd.setCursor(10, 1);
      lcd.print(">");
      lcd.setCursor(5, 1);
      lcd.print(" ");
      whatIsUserDoingToPet == "P";
      }
    if(whatIsUserDoingToPet == "P"){
      state = MENU;
      whatIsUserDoingToPet == "M";
      lcd.clear();
    }
  }
}

//Function for feeding
void Feed() {
  int period = 2;
  if (Fullness < 3 && Fullness > 0) {
    Fullness += 1;
    if (Happiness < 2) {
      Happiness += 1;
    }
    updateFullness();
    updateHappiness();
  } else if (Fullness == 3 || Fullness == 0) {
    Happiness = 0;
    Fullness += 1;
    updateHappiness();
    updateFullness();
  }
  state = IDLE;
}

//function that reduces the fullness every 11 seconds after the pet has hatched
void ReduceFullness() {
  int fullTime = timer + (minutes * 60) - 5;
  boolean fullnessreduced = false;
  if ((fullTime % 11) == 0 && fullTime > 0 && ((millis() / 1000) >= (currentTime + 1)) 
  && Fullness > 0 && !fullnessreduced) {
    Fullness--;
    updateFullness();
    fullnessreduced = true;
  }
}

//updates fullness on the lcd screen
void updateFullness() {
  lcd.setCursor(6, 0);
  lcd.print("F");
  lcd.print(Fullness);
}

//updates happiness on the lcd screen
void updateHappiness() {
  lcd.setCursor(3, 0);
  lcd.print("H");
  lcd.print(Happiness);
}

//reduces happiness after 5 seconds, every 7 seconds
void ReduceHappiness() {
  int fullTime = timer + (minutes * 60) - 5;
  if ((fullTime % 7) == 0 && fullTime > 0 && ((millis() / 1000) >= (currentTime + 1)) 
  && Happiness > 0) {
    Happiness--;
    updateHappiness();
  }
}

//this is where the happiness increases depending on the current happiness
void PlayWithPet() {
  if ((Fullness >= 2 && Fullness < 5) && Happiness < 2) {
    if (Happiness < 2) {
      Happiness++;
      updateHappiness();
    }
  }
  state = IDLE;
}

//grows pet when the stats satisfy the specification to grow
void GrowPet() {
  int fullTime = timer + (minutes * 60);
  if (Happiness >= 1 && fullTime >= 35 && Fullness >= 3) {
    developmentStage = 2;
    lcd.setCursor(0, 0);
    lcd.print("D");
    lcd.print(developmentStage); 
  }
  state = IDLE;
}

//what is shown when the pet starts up
void StartupSequences() {
  uint8_t menuFunctions = lcd.readButtons();
  String menuOptions[4];
  menuOptions[0] = "Save Pet";
  menuOptions[1] = "Close Menu";
  menuOptions[2] = "Delete Pet";
  menuOptions[3] = "New Pet";
  int menuOption = 0;

  lcd.setCursor(0,0);
  lcd.print("Choose an Option:");
  lcd.setCursor(0,1);
  lcd.print("Load<- ->New");
  if(menuFunctions & BUTTON_LEFT){
    lcd.clear();
    state = LOAD;
  }
  else if(menuFunctions & BUTTON_RIGHT){
  lcd.clear();
  state = NEW;
  }
}

//Function for load pet.
// feature explained in the documentation
void LoadPet(){
  int loadPetDetails[8];
  for(int i = 0; i<8; i++){
    loadPetDetails[i]=EEPROM.read(i);
  }
  if(loadPetDetails[1] == 104 && loadPetDetails[3] == 101 && loadPetDetails[5] == 99 ){
    if(loadPetDetails[0]<3 && loadPetDetails[2] < 5 && loadPetDetails[4] < 3 && loadPetDetails[6] < 10 && loadPetDetails[7] < 60){
      developmentStage = loadPetDetails[0];
      Fullness = loadPetDetails[2];
      Happiness = loadPetDetails[4];
      minutes = loadPetDetails[6];
      timer = loadPetDetails[7];
      limiter = false;
      state = IDLE;
    }
  } 
  else {
    state = NEW;
    }
}

//what happens when the state is equal to NEW
void newPet(){
  Serial.println("newPET");
  developmentStage = 0;
  Happiness = 2;
  Fullness = 3;
  timer = 0;
  minutes = 0;
  limiter = false;
  state = IDLE;
}

//Menu when accessed when running the pet.
void Menu(){
  uint8_t menuFunctions = lcd.readButtons(); 
  //basic layout for the menu
  lcd.setCursor(1,0);
  lcd.print("Back");
  lcd.setCursor(6,0);
  lcd.print("Menu");
  lcd.setCursor(11,0);
  lcd.print("Save");
  lcd.setCursor(1,1);
  lcd.print("Delete");
  lcd.setCursor(13,1);
  lcd.print("New");
  //how the menu works
  if(whatIsUserDoingToPet == "P"){
    lcd.setCursor(0,0);
    lcd.print(">");
    whatIsUserDoingToPet = "B";
  }
  if(menuFunctions & BUTTON_RIGHT){
   if(whatIsUserDoingToPet == "B"){
    MenuDetailsSave();
   }
    if(whatIsUserDoingToPet == "D"){
      MenuDetailsNew();
      }
  }
  if (menuFunctions & BUTTON_LEFT){
    if(whatIsUserDoingToPet == "S"){
        MenuDetailsBack();
    }
      if(whatIsUserDoingToPet == "N"){
        MenuDetailsDelete();
      }
  }
  if (menuFunctions & BUTTON_DOWN){
    if(whatIsUserDoingToPet == "B"){
        MenuDetailsDelete();
      }
      if(whatIsUserDoingToPet == "S"){
        MenuDetailsNew();
      }
  }
  if (menuFunctions & BUTTON_SELECT){
      if(whatIsUserDoingToPet == "D"){
        state = DELETE;
      }
      if(whatIsUserDoingToPet == "B"){
        state = IDLE;
        removeChars();
      }
      if(whatIsUserDoingToPet == "S"){
        state = SAVE;
      }
      if(whatIsUserDoingToPet == "N"){
        state = NEW;
        removeChars();
      }
  }
  if (menuFunctions & BUTTON_UP){
      if(whatIsUserDoingToPet == "D"){
        MenuDetailsBack();
      }
      if(whatIsUserDoingToPet == "N"){
        MenuDetailsSave();
      }
  }
}

//how it saves the pet
//explained in the documentation
void SavePet(){
    char checking[] = {developmentStage, 'h', Fullness, 'e', Happiness, 'c', minutes, timer};
    for(int character=0; character<8; character++){
    EEPROM.write(character, checking[character]);
  }
    lcd.clear();
    state = SAVEDPET;
}

//what happens when the back option has been selected
void MenuDetailsBack(){
  lcd.setCursor(10,0);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print(" ");
  lcd.setCursor(12,1);
  lcd.print(" ");
  lcd.setCursor(0,0);
  lcd.print(">");
  whatIsUserDoingToPet = "B";
}

//what happens when the save option has been selected
void MenuDetailsSave(){
  lcd.setCursor(0,0);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print(" ");
  lcd.setCursor(12,1);
  lcd.print(" ");
  lcd.setCursor(10,0);
  lcd.print(">");
  whatIsUserDoingToPet = "S";
}

//what happens when the new option has been selected
void MenuDetailsNew(){
  lcd.setCursor(10,0);
  lcd.print(" ");
  lcd.setCursor(0,0);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print(" ");
  lcd.setCursor(12,1);
  lcd.print(">");
  whatIsUserDoingToPet = "N";
}

//what happens when the delete option has been selected
void MenuDetailsDelete(){
  lcd.setCursor(10,0);
  lcd.print(" ");
  lcd.setCursor(0,0);
  lcd.print(" ");
  lcd.setCursor(12,1);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print(">");
  whatIsUserDoingToPet = "D";
}
//eeprom has been written with the basic values
void DeletePet(){
  char deleting[] = {0, 'h', 3, 'e', 2, 'c', 0, 0};
  for(int character=0; character<8; character++){
    EEPROM.write(character, deleting[character]);
  }
  developmentStage = 0;
  Fullness = 3;
  Happiness = 2;
  state = MENU;
}
//removes the extra characters that are still present when you from the menu back to the pet.
void removeChars(){
  lcd.setCursor(8,0);
        lcd.print("  ");
        lcd.setCursor(5,1);
        lcd.print(" ");
        lcd.setCursor(15,1);
        lcd.print(" ");
}
      
