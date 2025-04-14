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

bool compare(int index){
  if (simonSays.substring(0, index).equals(youSay)) return true;
  Serial.println("SimonSays: " + simonSays);
  return false;
}

void listenSimon(){
  for (int i = 0; i < simonSays.length(); i++){
    digitalWrite(simonSays[i] - '0' + 2, HIGH);
    delay(500);
    digitalWrite(simonSays[i] - '0' + 2, LOW);
    delay(500);
  }
}

void say(){
  int count = 1;
  while (count <= simonSays.length()){
    youSay += input();
    Serial.println(youSay);

    if (compare(count) == false){
      gameOver();
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

void advance(){
  delay(400);
  youSay = "";
  incScore();
  successSound();
  simonSays += random(0, 4); 

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
        data = '0';
      }
      buttonStateR = readR;
      if (readG == LOW && buttonStateG == HIGH){
        data = '1';
      }
      buttonStateG = readG;
      if (readB == LOW && buttonStateB == HIGH){
        data = '2';
      }
      buttonStateB = readB;
      if (readY == LOW && buttonStateY == HIGH){
        data = '3';
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

int score = 0;
int highScore = 0;
void incScore(){
  score++;
}
void resetScore(){
  score = 0;
}
void initHighScore(){
  highScore = EEPROM.read(0);
  Serial.println("EEPROM: " + (String)EEPROM.read(0));
}
void updateHighScore(){
  if (score > highScore){
    EEPROM.update(0, score - 1);
    highScore = EEPROM.read(0);
    Serial.println("EEPROM UPDATED: " + (String)EEPROM.read(0));
  }
}
void dispScore(){
  lcd.setCursor(0,0);
  lcd.print("SCORE: " + (String)score);
  lcd.setCursor(0,1);
  lcd.print("HIGH SCORE: " + (String)highScore);
}

void setup() {
  EEPROM.write(0, 0);
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
  delay(2000);

  simonSays += random(0, 4); 

}

void loop() {

  listenSimon();
  say();
  dispScore();
  advance();

}
