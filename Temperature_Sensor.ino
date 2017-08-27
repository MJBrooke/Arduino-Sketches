#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

const int temperatureSensorPin = A0;

const size_t deviceInformationBufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + 310;
const size_t temperatureBufferSize = JSON_OBJECT_SIZE(1) + 20;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

BridgeServer server;

int timeUntilLcdTemperatureRefresh = 0;

void setup() {
  lcd.begin(16,2);
  lcd.setCursor(11,0);
  lcd.print("Alpha");

  lcd.setCursor(1,1);
  lcd.print("-Setting Up-");

  Bridge.begin();
  
  server.begin();
}

void loop() {
  BridgeClient client = server.accept();

  if(client) {
    processRestRequest(client);
    client.stop();
  }

  if(timeUntilLcdTemperatureRefresh == 0) {
    refreshLcdTempReading();
    timeUntilLcdTemperatureRefresh = 5000;
  }

  timeUntilLcdTemperatureRefresh -= 50;
  delay(50);
} 

void processRestRequest(BridgeClient client) {
  String command = client.readString();
  command.trim();

  if(command == "deviceInformation") {
    writeResponse(client, createDeviceInformationJson());
  } else if (command == "temperature") {
    writeResponse(client, createTemperatureJson());
  }
}

void writeResponse(BridgeClient& client, JsonObject& json) {
  client.println("Status: 200");
  client.println("Content-type: application/json");
  client.println("Connection: close");
  client.println();
  json.printTo(client);
}

JsonObject& createDeviceInformationJson() {
  DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["name"] = "Temperature Sensor";
  root["description"] = "Reads the ambient temperature of the surrounding atmosphere";
  root["endpointRoot"] = "temperatureSensor";  
  root["componentType"] = "Sensor";

  JsonArray& capabilities = root.createNestedArray("capabilities");

  JsonObject& readTemperatureCapability = capabilities.createNestedObject();

  readTemperatureCapability["name"] = "Read Temperature";
  readTemperatureCapability["description"] = "Reads the temperature from the surrounding atmosphere";
  readTemperatureCapability["temperature"] = "temperature";

  return root;
}

JsonObject& createTemperatureJson() {
  DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["response"] = String(getTempInCelsius());

  return root;
}

float getTempInCelsius() {
  int analogTempReading = analogRead(temperatureSensorPin);
  
  float voltage = (analogTempReading/1024.0) * 5;
  
  float tempCelsius = (voltage - .5) * 100;

  return tempCelsius;
}

void refreshLcdTempReading() {
  lcd.setCursor(0,1);
  lcd.print("          ");
  lcd.print(getTempInCelsius());
  lcd.print("C");
}