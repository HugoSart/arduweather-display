#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal.h>
#include <stdio.h>

const int buttonPin = 8;
const int serialSize = 64; // primeiro é temp, seg é umidade, terceiro timestamp, quarto string ate \0
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int buttonState = HIGH;
int reading;
int previous = LOW;
int segundo = 0;
int minuto = 0;

time_t lastTime;
long timer = 0;
long debounce = 200;

// [0] byte temperatura, [1] byte umidade, [2-9] long timestamp, [10 até \0] string previsão do tempo.
byte serialData[serialSize] = {10,20,0,0,0,0,0,0,0,0,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};

boolean readSerial = true;
boolean tempScreen = false;

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  loading_screen();
  delay(5000);
  read_serial();
  delay(2000);
  read_serial();
}

void loading_screen() {

  lcd.setCursor(0,0);
  lcd.print("Arduweather   SD");
  lcd.setCursor(0,1);
  lcd.print("Loading      ...");
  
}

void switch_screen(){
  
  reading = digitalRead(buttonPin);

  if(reading == HIGH && previous == LOW && millis() - timer > debounce){
    if(buttonState == HIGH) {
      buttonState = LOW;
      tempScreen = true;
    } else {
      buttonState = HIGH;
      tempScreen = false;
    }
    timer = millis();
  }
  if(tempScreen == true){
    temperature_screen();
  }
  else{
    data_screen();
  }
  previous = reading;
  
}

void temperature_screen(){
  
  char c[16] = "                ";
  lcd.setCursor(0, 0);
  sprintf(c, " %3d C / H %2d %% ", serialData[0], serialData[1]);
  lcd.print(c);
  
  lcd.setCursor(0, 1);
  int i;
  for (i = 0; i < 16 && serialData[i + 10] != '\0'; i++)
    lcd.write(serialData[i + 10]);
  for (; i < 16; i++)
    lcd.write(' ');  
  
}

void data_screen(){
  
  time_t t = now();
  int sec = second(t);
  int minu = minute(t);
  int h = hour(t);
  int dia = day(t);
  int mes = month(t);
  int ano = year(t);
  char c[16];
  lcd.setCursor(0, 0);
  sprintf(c, "    %02d:%02d:%02d    ", h, minu, sec);
  lcd.print(c);
  lcd.setCursor(0, 1);
  sprintf(c, "   %02d/%02d/%04d   ", dia, mes, ano);
  lcd.print(c);
  
}

void update_data(int8_t data[]) {

  int64_t value =  (int64_t)serialData[2] << 56 |
                   (int64_t)serialData[3] << 48 |
                   (int64_t)serialData[4] << 40 |
                   (int64_t)serialData[5] << 32 |
                   (int64_t)serialData[6] << 24 |
                   (int64_t)serialData[7] << 16 |
                   (int64_t)serialData[8] << 8  |
                   (int64_t)serialData[9];
  
  time_t t = value;
  setTime(t);
   
}

bool read_serial(){
  Serial.print(1);

  for (int i = 0; i < 64; i++)
    serialData[i] = 0;
  
  int n = Serial.readBytesUntil('\n', serialData, 64);
  for (int i = 0; i < 64; i++)
    Serial.print(serialData[i]);
  update_data(serialData);
  return true;
}

void loop() {

  time_t t = now();
  if (t - lastTime > 3600) {
    read_serial();
    lastTime = t;
  }
  
  switch_screen();
  
}
