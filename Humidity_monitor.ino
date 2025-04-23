
#include "LiquidCrystal_I2C.h"          


#define solPin1 A1 // порт для подключения датчика
#define solPin2 A2 // порт для подключения датчика
#define solPin3 A3 // порт для подключения датчика
LiquidCrystal_I2C lcd(0x27, 16, 2);

int s1min=565;
int s1max=289;

int s2min=558;
int s2max=0;

int s3min=1023;
int s3max=480;
void setup() {
  Serial.begin(9600); // --------- Консоль ---------
  while (! Serial) {
    delay(1);
  }
  pinMode(solPin1, INPUT);
  pinMode(solPin2, INPUT);
  pinMode(solPin3, INPUT);


  lcd.init();
  lcd.backlight();



}
 
void loop() {
  // считываем данные с датчика влажности почвы
  int sol1 = analogRead(solPin1);
   int sol2 = analogRead(solPin2);
    int sol3 = analogRead(solPin3);
  
  Serial.print("D1-");
  Serial.println(sol1);
    Serial.print("D2-");
  Serial.println(sol2);
    Serial.print("D3-");
  Serial.println(sol3);
   Serial.println("---------------------------------");
  // ждём 100 мс
 


  lcd.setCursor(0, 0); // Устанавливаем положение курсора, 0-й, 0-я строка
  lcd.print("Humidity sensor"); // Выводим текст
lcd.setCursor(0, 1);
lcd.print(sol1);
  lcd.setCursor(4, 1);
  lcd.print("/");
 
  lcd.setCursor(6, 1);
 lcd.print(sol2);
  lcd.setCursor(10, 1);
  lcd.print("/");
  
  lcd.setCursor(12, 1);
 lcd.print(sol3);
 
 delay(3000);
  lcd.clear();  //Чистим экран
  lcd.setCursor(0, 0); // Устанавливаем положение курсора, 0-й, 0-я строка
  lcd.print("Humidity in %"); // Выводим текст
lcd.setCursor(0, 1);
lcd.print(map(sol1, s1min, s1max, 0, 100));
  lcd.setCursor(4, 1);
  lcd.print("/");
   lcd.setCursor(6, 1);
lcd.print(map(sol2, s2min, s2max, 0, 100));
  lcd.setCursor(10, 1);
  lcd.print("/");
    lcd.setCursor(12, 1);
 lcd.print(map(sol3, s3min, s3max, 0, 100));
 
 delay(3000);
  lcd.clear();  //Чистим экран
}
