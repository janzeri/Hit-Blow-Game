#include <LiquidCrystal.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
// initialize the library with the numbers of the interface pins
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00000000,
    B01000010,
    B10100101,
    B00000000,
    B00000000,
    B00100100,
    B00011000,
    B00000000 },
  eye_bmp[] =
  { B00000000,
    B01000010,
    B10100101,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000 },
  laugh_bmp[] =
  { B00000000,
    B01000010,
    B10100101,
    B00000000,
    B01111110,
    B01000010,
    B00111100,
    B00000000 },
  neutral_bmp[] =
  { B00000000,
    B00000000,
    B00100100,
    B00000000,
    B00000000,
    B00100100,
    B00011000,
    B00000000 },
  sad_bmp[] =
  { B00000000,
    B00100100,
    B01000010,
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B00000000 },
  bikkuri_bmp[] =
  { B00000000,
    B01000010,
    B00000000,
    B00000000,
    B00111100,
    B00100100,
    B00100100,
    B00111100 },
  anger_bmp[] =
  { B00000000,
    B01000010,
    B00100100,
    B00000000,
    B00111100,
    B00100100,
    B00111100,
    B00111100 };
    
int JoyStick_X = 0;
int JoyStick_Y = 1;
int JoyStick_Z = 2;
 
LiquidCrystal lcd(11, 12, 6, 7, 4, 5);

void setup() {
  Serial.begin(9600);
  matrix.begin(0x70);

  pinMode(JoyStick_Z, INPUT);
  randomSeed(analogRead(3));
  lcd.setCursor(0, 0);
  lcd.print("Hit&Blow");
  lcd.setCursor(0, 1);
  lcd.print("  Game  "); 
}

String hit(int input[3], int ans[3], bool *done){
  int h = 0;
  for(int i = 0; i < 3; i++){
    if(input[i] == ans[i]){
      h++;
      done[i] = true;
    }
  }
  return String(h);
}

String blow(int input[3], int ans[3], bool *done){
  int b = 0;
  for(int i = 0; i < 3; i++){
    if(done[i]) continue;
    for(int j = 0; j < 3; j++){
      if(done[j]) continue;
      if(input[i] == ans[j]){
        b++;
        done[j] = true;
      }
    }
  }
  return String(b);
}

int state = 0, cnt = 0, p = 0, flag = 0;
int input[3], ans[3];
bool done[3];
String num;

void loop() {
  int x, y, z;
  static int pz;
  static unsigned long t = millis();
  x = analogRead(JoyStick_X);
  y = analogRead(JoyStick_Y);
  z = digitalRead(JoyStick_Z);

  if(state == 0){
    matrix.clear();
    matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    lcd.setCursor(0, 0);
    lcd.print("Hit&Blow");
    lcd.setCursor(0, 1);
    lcd.print("  Game");
    if(pz == 0 && z == 1){
      ans[0] = random(0,9);
      ans[1] = random(10);
      while(ans[0] == ans[1]){
        ans[1] = random(10);
      }
      ans[2] = random(10);
      while(ans[0] == ans[2] || ans[1] == ans[2]){
        ans[2] = random(10);
      }
      for(int i = 0; i < 3; i++){
        done[i] = false;
      }
      lcd.setCursor(0, 0);
      lcd.print("  Good  ");
      lcd.setCursor(0, 1);
      lcd.print("  Luck! ");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.print("        ");
      input[0] = 0;
      input[1] = 0;
      input[2] = 0;
      p = 0;
      state++;
    }
  } else if(state == 1){
    matrix.clear();
    if(cnt > 6 && cnt < 10){
      matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
    } else if(cnt >= 10){
      matrix.drawBitmap(0, 0, laugh_bmp, 8, 8, LED_ON);
    } else {
      matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
    }
    matrix.writeDisplay();
    if(cnt < 10)lcd.setCursor(7, 1);
    else lcd.setCursor(6, 1);
    lcd.print(String(cnt));
    if(cnt == 0){
      lcd.setCursor(0, 0);
      lcd.print("--- -H-B");
    } else {
      lcd.setCursor(0, 0);
      lcd.print(num);
    }
    
    if(x >= 950){
      if(p < 2)p++;
    } else if(x <= 50){
      if(p > 0)p--;
    } else if(y >= 950){
      if(input[p] < 9)input[p]++;
    } else if(y <= 50){
      if(input[p] > 0)input[p]--;
    }
    //UPDATE NUMBER below
    for(int i = 0; i < 3; i++){
      if(i == p){
        if(millis() - t > 500){
          lcd.setCursor(p, 1);
          if(flag == 0){
            lcd.print(" ");
            flag = 1;
          } else {
            lcd.print(String(input[p]));
            flag = 0;
          }
          t = millis();
        }
      } else {
        lcd.setCursor(i, 1);
        lcd.print(String(input[i]));
      }
    }

    //confirm number
    if(pz == 0 && z == 1){
      if(ans[0] == ans[1] || ans[1] == ans[2] || ans[0] == ans[2]);
      else {
        if(cnt < 99)cnt++;
        if(hit(input, ans, done) == "3")state = 3;
        else state = 2;
        //Serial.print("ans = ");Serial.print(ans[0]);Serial.print(ans[1]);Serial.println(ans[2]); //debug
      }
    }
  } else if(state == 2){
    matrix.clear();
    matrix.drawBitmap(0, 0, eye_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    lcd.setCursor(0, 0);
    lcd.print(" Wrong  ");
    num = String(input[0]) + String(input[1]) + String(input[2]) + " " + hit(input, ans, done) + "H" + blow(input, ans, done) + "B";
    lcd.setCursor(0, 1);
    lcd.print(num);
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("        ");
    state = 1;
    for(int i = 0; i < 3; i++){
        done[i] = false;
     }
  } else if(state == 3){
    //congrats!
    matrix.clear();
    if(cnt == 1){
      matrix.drawBitmap(0, 0, anger_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
    }else if(cnt <= 3){
      matrix.drawBitmap(0, 0, bikkuri_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
    }else {
      matrix.drawBitmap(0, 0, sad_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
    }
    if(millis() - t > 1500){
          lcd.setCursor(p, 1);
          if(flag == 0){
            lcd.setCursor(0, 0);
            lcd.print("Congrats");
            lcd.setCursor(0, 1);
            lcd.print("You win!");
            flag = 1;
          } else {
            lcd.setCursor(0, 0);
            lcd.print("Press   ");
            lcd.setCursor(0, 1);
            lcd.print("Button  ");
            flag = 0;
          }
          t = millis();
    }
        
    if(pz == 0 && z == 1){
      state = 0;
      cnt = 0;
      matrix.clear();
    }
  }

  pz = z;
  delay(150);
}
//////////////////////////////////////////////////////////
