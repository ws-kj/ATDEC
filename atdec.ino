//ATDEC - Will Savage

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <SPI.h>
#include "RF24.h"

#define pir 2

#define rs  3
#define  e   4
#define d4  5
#define d5  6
#define d6  7
#define d7  8
  
#define red     9 
#define green   10
#define blue    11

#define buzzer  12

#define ce    A7
#define csn   A6
#define mos   A3
#define sck   A2
#define irq   A1
#define misc  A0

#define ser A2

//RF24 radio(ce, csn);

Servo servo;

int pirLowIn;
bool pirLockLow;
bool pirTakeLowTime;

int pirState = LOW;
int pirVal;

int minTime = 500;

int magic = 10;

bool inUse;

const byte addr[6] = "00001";
byte data = 0; //0 = safe /// 1 = not safe

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal lcd = LiquidCrystal(rs, e, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);

  mlx.begin();
  
  lcd.begin(16, 2);
  
  pinMode(pir, INPUT);

  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);

  pinMode(buzzer, OUTPUT);

  servo.attach(ser);
  servo.write(0);

  //radio.begin();
  //radio.openWritingPipe(addr);
 
  //radio.stopListening();

  rgbColor(0, 0, 255);

  Serial.println("TempEye started");

  delay(1000);
  
}

void loop() {
  readPir();
  displayIdle();
  
}

void readPir() {
  delay(200);
  pirVal = digitalRead(pir);
  Serial.println(pirVal);
  if(pirVal == HIGH) {
    Serial.println("eee");
    if(pirState == LOW) {
      pirState = HIGH;
      Serial.println("User has entered range");
      rgbColor(255, 255, 0);
      lcd.clear();
      processUser();
      
    }
  }
}

void displayTemp() {
  lcd.setCursor(0, 0);
  lcd.print("User temp:");
  lcd.setCursor(0, 1);
  lcd.print((mlx.readObjectTempF() + 7));
  lcd.print(" *F");
}

void displayIdle() {
  rgbColor(0, 0, 255);
  lcd.setCursor(0, 0);
  lcd.print("Waiting for user...");
  lcd.setCursor(0, 1);
  lcd.print("Uptime (s): ");
  lcd.print(millis() / 1000);
  lcd.print("   ");
}

void rgbColor(char r, char g, char b) {
  analogWrite(red, r);
  analogWrite(green, g);
  analogWrite(blue, b);
}

void processUser() {
  lcd.clear();
  lcd.print("Reading temp...");
  
  int t1 = millis();
  while(millis() < t1 + 5000) {
    lcd.setCursor(0, 1);
    lcd.print(mlx.readObjectTempF() + magic);
    lcd.print(" *F");
    if(mlx.readObjectTempF() + magic > 100) {
      highTemp();
      return;
    }
  }
  rgbColor(0, 255, 0);
  lcd.clear();
  lcd.print("Safe user temp.");
  beep(2, 1000, 100);
  data = 0;
  //radio.write(&data, sizeof(data));
  
  
  pirState = LOW;
  servo.write(180);
  delay(10000);
  servo.write(0);
}

void highTemp() {
  lcd.clear();
  lcd.print("WARNING:");
  lcd.setCursor(0, 1);
  lcd.print("High user temp.");
  rgbColor(255, 10, 10);

  beep(3, 700, 250);
  data = 1;
//  radio.write(&data, sizeof(data));
  servo.write(0);
  delay(5000);
}

void beep(int i, int khz, int t) {
  for(int k=0; k<i; k++) {
    tone(buzzer, khz);
    delay(t);
    noTone(buzzer);
    delay(t);
  }
}
