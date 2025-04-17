#include <LiquidCrystal.h>
#include <EEPROM.h>

const int rs = 12, e = 11, d4 = A0, d5 = A1, d6 = A2, d7 = A3;
LiquidCrystal lcd(rs, e, d4, d5, d6, d7);

const int buzzer = 10;
const int ledR = 5, ledG = 4, ledB = 3, ledY = 2;

const int buttonR = 9, buttonG = 8, buttonB = 7, buttonY = 6;

int buttonStateR = HIGH, buttonStateG = HIGH, buttonStateB = HIGH, buttonStateY = HIGH;
int lastButtonStateR = HIGH, lastButtonStateG = HIGH, lastButtonStateB = HIGH, lastButtonStateY = HIGH;

int lastDebounceTime;
int debounceTime = 50;

String simonSays = "";
String youSay = "";

int score = 0;
int highScore = 0;

int gameSpeed = 500; //500 by default, delay between leds. lower value = game starts harder can't be less than 150.


bool compare(int index){
  if (simonSays.substring(0, index).equals(youSay)) return true;
  return false;
}

void listenSimon(){
  for (int i = 0; i < simonSays.length(); i++){
    digitalWrite(simonSays[i] - '0' + 2, HIGH);
    delay(gameSpeed);
    digitalWrite(simonSays[i] - '0' + 2, LOW);
    delay(gameSpeed);
  }
}

void say(){
  int count = 1;
  while (count <= simonSays.length()){
    youSay += input();

    if (compare(count) == false){
      gameOver();
      WaitForStart();
      break;
    }
    count++;
  }
}

void gameOver(){
  digitalWrite(13, HIGH);
  failSound();
  digitalWrite(13, LOW);

  simonSays = "";
  youSay = "";
  updateHighScore();
  resetScore();
}

float gameSpeedMultiplier = 2;
void advance(){
  delay(400);
  youSay = "";
  incScore();
  successSound();
  simonSays += random(0, 4); 


  if (gameSpeed > 150){
    gameSpeed = 500 - 10*log(gameSpeedMultiplier); 
  }
  else{
    gameSpeed = 150;
  }
  gameSpeedMultiplier *= 2;
}

char input(){
  char data = 'e';
  while(true){
    int readR = digitalRead(buttonR);
    int readG = digitalRead(buttonG);
    int readB = digitalRead(buttonB);
    int readY = digitalRead(buttonY);

    if (readR != lastButtonStateR || readG != lastButtonStateG || readB != lastButtonStateB || readY != lastButtonStateY){
      lastDebounceTime = millis();
    }

    if (millis() - lastDebounceTime > debounceTime){

      if (readR == LOW && buttonStateR == HIGH){
        data = '3';
      }
      buttonStateR = readR;
      if (readG == LOW && buttonStateG == HIGH){
        data = '2';
      }
      buttonStateG = readG;
      if (readB == LOW && buttonStateB == HIGH){
        data = '1';
      }
      buttonStateB = readB;
      if (readY == LOW && buttonStateY == HIGH){
        data = '0';
      }
      buttonStateY = readY;
    }

    lastButtonStateR = readR;
    lastButtonStateG = readG;
    lastButtonStateB = readB;
    lastButtonStateY = readY;
    
    if (data != 'e'){
      inputSound();

      return data;
    }
  }
}

void successSound(){
  int i = 250;
  int mul = 1;
  while (i < 4000){
    tone(buzzer, i);
    i += 40 * mul;
    mul *= 2;
    delay(40);
    noTone(buzzer);
    delay(40);
  }
}

void failSound(){
  int i = 250;
  int mul = 1;
  while (i > 50){
    tone(buzzer, i);
    i -= 36 * mul;
    mul *= 1.4f;
    delay(300);
    noTone(buzzer);
    delay(100);
  }
}

void inputSound(){
  tone(buzzer, 600);
  delay(150);
  noTone(buzzer);
}

void incScore(){
  score++;
}

void resetScore(){
  score = -1;
}

void initHighScore(){
  highScore = EEPROM.read(0);
}

void updateHighScore(){
  if (score > highScore){
    EEPROM.update(0, score);
    highScore = EEPROM.read(0);
  }
}

void dispScore(){
  lcd.setCursor(0,0);
  lcd.print("SCORE: " + (String)(score));
  lcd.setCursor(0,1);
  lcd.print("HIGH SCORE: " + (String)(highScore));
}

void VeryFirstInit(){
  int eepromLength = EEPROM.length();
  if (EEPROM.read(512) != "\0"){
    for (int i = 0; i < eepromLength; i++){
      EEPROM.write(i, "\0");
    }
    EEPROM.write(0, 0);
  }
}

void WaitForStart(){
  ClearLCD();

  lcd.setCursor(0, 0);
  lcd.print("Press Any Key");
  lcd.setCursor(0, 1);
  lcd.print("To Start Game");

  while (true){
    char inp = input();
    if (inp == '0' || inp == '1' || inp == '2' || inp == '3'){
      break;
    }
  }

  ClearLCD();
}

void ClearLCD(){
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

}


void setup() {
  VeryFirstInit(); // This function only works if it is arduino's first initilization to clear all EEPROM Memory
  pinMode(ledR, INPUT);
  pinMode(ledG, INPUT);
  pinMode(ledB, INPUT);
  pinMode(ledY, INPUT);
  pinMode(buzzer, INPUT);

  pinMode(buttonR, INPUT_PULLUP);
  pinMode(buttonG, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonY, INPUT_PULLUP);

  Serial.begin(9600);






  initHighScore();
  lcd.begin(16,2);
  lcd.print("Hello");
  delay(1000);

  WaitForStart();
  randomSeed(millis());
  simonSays += random(0, 4); 
}

void loop() {
  listenSimon();
  say();
  advance();
  dispScore();
}
