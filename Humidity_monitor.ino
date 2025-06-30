#include "LiquidCrystal_I2C.h"

// Порты подключения датчиков влажности почвы
#define SOL_PIN1 A0
#define SOL_PIN2 A1
#define SOL_PIN3 A2

// Адрес I2C-дисплея и размеры строки и столбца
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Значения диапазонов для каждого датчика
struct SensorRange {
  int min;
  int max;
};

// Массив структур с диапазонами для всех трёх датчиков
SensorRange ranges[] = {{565, 289}, {558, 0}, {1023, 480}};

void setup() {
  Serial.begin(9600); // Начало связи с последовательным монитором
  while (!Serial) {
    delay(1);
  }

  // Устанавливаем датчики на вход
  for (int i = 0; i <= 2; ++i) {
    pinMode(i + A0, INPUT);
  }

  // Инициализация LCD-экрана
  lcd.init();
  lcd.backlight();
}

void loop() {
  // Читаем показания с датчиков
  int values[3];
  for (int i = 0; i < 3; ++i) {
    values[i] = analogRead(i + A0);
  }

  // Печать сырых данных в монитор порта
  printRawValues(values);

  // Печать преобразованных процентов влажности на экране
  printMappedValues(values);

  delay(3000); // Пауза перед повторением цикла
}

// Функция вывода сырых данных на экран и в консоль
void printRawValues(const int values[]) {
  Serial.print("D1:");
  Serial.println(values[0]);
  Serial.print("D2:");
  Serial.println(values[1]);
  Serial.print("D3:");
  Serial.println(values[2]);
  Serial.println("------------------------");

  lcd.setCursor(0, 0);
  lcd.print("Humidity sensors");
  lcd.setCursor(0, 1);
  lcd.print(values[0]); lcd.print("/");
  lcd.print(values[1]); lcd.print("/");
  lcd.print(values[2]);

  delay(3000);
  lcd.clear();
}

// Преобразование сырого сигнала в проценты влажности и вывод на экран
void printMappedValues(const int values[]) {
  lcd.setCursor(0, 0);
  lcd.print("Humidity in %");
  lcd.setCursor(0, 1);

  for (int i = 0; i < 3; ++i) {
    int mappedValue = map(values[i], ranges[i].min, ranges[i].max, 0, 100);
    lcd.print(mappedValue);
    if (i < 2) {
      lcd.print("/");
    }
  }

  delay(3000);
  lcd.clear();
}
