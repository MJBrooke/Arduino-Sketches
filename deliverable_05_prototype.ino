#include <LiquidCrystal.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

const int temperatureSensorPin = A0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

BridgeServer server;

int timeUntilLcdTemperatureRefresh = 0;

void setup() {
  Serial.begin(9600);

  lcd.begin(16,2);
  lcd.print("Prototype - D05");

  lcd.setCursor(1,1);
  lcd.print("-Setting Up-");

  Bridge.begin();
  
  server.begin();
}

void loop() {
  BridgeClient client = server.accept();

  if(client) {
    process(client);
    client.stop();
  }

  if(timeUntilLcdTemperatureRefresh == 0) {
    refreshLcdTempReading();
    timeUntilLcdTemperatureRefresh = 5000;
  }

  timeUntilLcdTemperatureRefresh -= 50;
  delay(50);
} 

void process(BridgeClient client) {
  String command = client.readString();
  command.trim();

  if(command == "temperature") {
    getTemperature(client);
  }
}

void refreshLcdTempReading() {
  float tempCelsius = getTempInCelsius();
  
  lcd.setCursor(1,1);
  lcd.print("Temp: ");
  lcd.print(tempCelsius);
  lcd.print("C");
}

void getTemperature(BridgeClient client) {
  float tempCelsius = getTempInCelsius();

  client.print(F("Current temperature: "));
  client.print(tempCelsius);
  client.print(F("C"));
}

float getTempInCelsius() {
  int analogTempReading = analogRead(temperatureSensorPin);
  
  float voltage = (analogTempReading/1024.0) * 5;
  
  float tempCelsius = (voltage - .5) * 100;

  return tempCelsius;
}

