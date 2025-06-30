#include <TM1637Display.h>   // Библиотека дисплея
#include <RotaryEncoder.h>   // Библиотека энкодера
#include <SD.h>              // Библиотека SD карты
#include <DHT.h>             // Библиотека датчика температуры/влажности

// Подключение компонентов
#define PIN_DHT 8            
#define DHTTYPE DHT11        
#define RELAY_PIN 7          
#define ENCODER_A 2          
#define ENCODER_B 3
#define BUTTON_PIN 4         
#define CLK_PIN 10           
#define DIO_PIN 11
#define SOIL_SENSOR_1 A0     // ДАТЧИКИ ПОЧВЫ
#define SOIL_SENSOR_2 A1
#define SOIL_SENSOR_3 A2
#define CS_SD_CARD 6         

// Инициализация библиотек
DHT dht(PIN_DHT, DHTTYPE);
TM1637Display display(CLK_PIN, DIO_PIN);
RotaryEncoder encoder(ENCODER_A, ENCODER_B);
File logfile;

// Глобальные переменные
const int maxMenuItems = 3;                  // Количество элементов меню
volatile byte currentMenuItem = 0;           // Текущий выбранный элемент меню
unsigned long timerStartTime = millis();     // Таймер старта
bool pumpState = false;                      // Флаг активности помпы
float soilMoistureThreshold = 500;           // Порог влаги в почве
int airHumidityThreshold = 60;               // Порог влажности воздуха
int wateringDuration = 10;                   // Продолжительность полива (в секундах)
unsigned long lastWateringTime = 0;          // Последняя запись данных
unsigned long waterInterval = 3600000;       // Интервал автоматического полива (1 час)

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  if (!SD.begin(CS_SD_CARD)) {
    Serial.println("Ошибка SD!");
    while (true); 
  }
  display.setBrightness(0x0F); 
  display.showNumberDec(0); 
  dht.begin();
}

void loop() {
  updateDisplay();               
  checkEncoderAndButton();        
  manageModes();                  
  recordSensorValues();           
}


void updateDisplay() {
  const char* menuNames[maxMenuItems] = {"MANUAL", "DURATION", "INTERVAL"};
  display.showString(menuNames[currentMenuItem], 0);
}


void checkEncoderAndButton() {
  static unsigned long lastButtonPress = 0;
  bool buttonPressed = !digitalRead(BUTTON_PIN);
  if (buttonPressed && (millis() - lastButtonPress > 500)) {
    lastButtonPress = millis();
    changeMenuItem(); 
  }

  int encDelta = encoder.getValue();
  if (encDelta != 0) {
    processEncoderChange(encDelta); 
    encoder.resetCount();
  }
}

void changeMenuItem() {
  currentMenuItem++;
  if (currentMenuItem >= maxMenuItems) currentMenuItem = 0;
}


void processEncoderChange(int delta) {
  switch (currentMenuItem) {
    case 0: togglePump(); break;                    
    case 1: adjustWateringDuration(delta); break;    
    case 2: adjustWateringInterval(delta); break;    
  }
}

void togglePump() {
  pumpState = !pumpState;
  digitalWrite(RELAY_PIN, pumpState ? HIGH : LOW);
}

void adjustWateringDuration(int delta) {
  wateringDuration += delta * 10; // Шаг регулировки 10 секунд
  constrain(wateringDuration, 10, 60); // Ограничиваем диапазон от 10 до 60 секунд
}


void adjustWateringInterval(int delta) {
  waterInterval += delta * 3600000; // Шаг регулировки 1 час
  constrain(waterInterval, 3600000, 86400000); // Минимум 1 час, максимум 24 часа
}


void manageModes() {
  if ((millis() - timerStartTime) >= waterInterval) {
    autoWaterPlants(); // Автополив
    timerStartTime = millis();
  }
}

void autoWaterPlants() {
  float moistureAvg = (analogRead(SOIL_SENSOR_1) +
                       analogRead(SOIL_SENSOR_2) +
                       analogRead(SOIL_SENSOR_3)) / 3.0;
  float humidityAir = dht.readHumidity();

  if (moistureAvg < soilMoistureThreshold ||
      humidityAir < airHumidityThreshold) {
    digitalWrite(RELAY_PIN, HIGH); // Включаем насос
    delay(wateringDuration * 1000); // Ждём установленное время
    digitalWrite(RELAY_PIN, LOW); // Выключаем насос
  }
}

void recordSensorValues() {
  if ((millis() - lastWateringTime) >= 3600000) {
    char filename[] = "DATA.CSV";
    logfile = SD.open(filename, FILE_WRITE);
    if (logfile) {
      logfile.print(dht.readTemperature()); logfile.print(',');
      logfile.print(dht.readHumidity()); logfile.print(',');
      logfile.print(analogRead(SOIL_SENSOR_1)); logfile.print(',');
      logfile.print(analogRead(SOIL_SENSOR_2)); logfile.print(',');
      logfile.println(analogRead(SOIL_SENSOR_3));
      logfile.close();
    }
    lastWateringTime = millis();
  }
}
