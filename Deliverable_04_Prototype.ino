#include <LiquidCrystal.h>

const int temperatureSensorPin = A0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);

  lcd.begin(16,2);
  lcd.print("Deliverable 04");
}

void loop() {
  int analogTempReading = analogRead(temperatureSensorPin);
  Serial.print("Analog Reading: ");
  Serial.print(analogTempReading);

  float voltage = (analogTempReading/1024.0) * 5;
  Serial.print(" | Voltage Conversion: ");
  Serial.print(voltage);

  float tempCelsius = (voltage - .5) * 100;
  Serial.print(" | Celsius: ");
  Serial.print(tempCelsius);

  lcd.setCursor(1,1);
  lcd.print("Temp: ");
  lcd.print(tempCelsius);
  lcd.print("C");

  delay(5000);
}
