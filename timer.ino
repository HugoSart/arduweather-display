#include <Time.h>
#include <TimeLib.h>
#include <TimerOne.h>
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

long timer = 0;
long debounce = 200;

// [0] byte temperatura, [1] byte umidade, [2-9] long timestamp, [10 até \0] string previsão do tempo.
byte serialData[serialSize] = {10,20,0,0,0,0,0,0,0,0,'a','a','a','a','a','a','a','a','a','a','a','a','a','\0'};

boolean readSerial = true;
boolean tempScreen = true;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);
  
  Serial.begin(9600);

  lcd.begin(16, 2);
  //lcd.print();

  uint64_t value = (uint64_t)serialData[2] |
                   (uint64_t)serialData[3] << 8 |
                   (uint64_t)serialData[4] << 16 |
                   (uint64_t)serialData[5] << 24 |
                   (uint64_t)serialData[6] << 32 |
                   (uint64_t)serialData[7] << 40 |
                   (uint64_t)serialData[8] << 48 |
                   (uint64_t)serialData[9] << 56;
  
  time_t t = value;
  setTime(t);

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

int read_serial(){
  Serial.write(1);
  for(int i = 0; i < serialSize; i++){
    if(Serial.available() <= 0){
      Serial.print("Erro na leitura do serial");
      return -1;
    }
    else
      serialData[i] = Serial.read();
  }
  return 1;
}

void temperature_screen(){
  char c[16];
  lcd.setCursor(0, 0);
  sprintf(c, " %3d C / H %3d%%", serialData[0], serialData[1]);
  lcd.print(c);
  lcd.setCursor(0, 1);
  for(int i = 0; i < 16; i++){
    c[i] = serialData[i+10];
    if(c[i] == '\0')
      break;
  }
  lcd.print(c);
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
  sprintf(c, "    %02d:%02d:%02d   ", h, minu, sec);
  lcd.print(c);
  lcd.setCursor(0, 1);
  sprintf(c, "   %02d/%02d/%04d   ", dia, mes, ano);
  lcd.print(c);
  
}

void ready_serial_again(){
  if(minuto == 59 and segundo == 59)
    readSerial = true;
}

void loop() {
  switch_screen();
  //condição para ler serial
  /*if(readSerial){
    readSerial = false;
    read_serial();
  }*/
}
