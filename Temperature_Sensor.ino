#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

const int temperatureSensorPin = A0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

BridgeServer server;

int timeUntilLcdTemperatureRefresh = 0;

void setup() {
  lcd.begin(16,2);
  lcd.setCursor(12,0);
  lcd.print("Beta");

  lcd.setCursor(6,1);
  lcd.print("Setting Up");

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

  if (command == "temperature") {
    writeResponse(client, createTemperatureJson());
  } else if(command == "deviceInformation") {
    writeResponse(client, createDeviceInformationJson());
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
  root["deviceId"] = "745821";
  root["name"] = "Temperature Sensor";
  root["description"] = "Reads the ambient temperature of the surrounding atmosphere";
  root["endpointRoot"] = "arduino";
    
  JsonObject& componentType = root.createNestedObject("componentType");
  componentType["name"] = "Sensor";
  componentType["id"] = "1";

  JsonArray& capabilities = root.createNestedArray("capabilities");

  JsonObject& readTemperatureCapability = capabilities.createNestedObject();

  readTemperatureCapability["name"] = "Read Temperature";
  readTemperatureCapability["description"] = "Reads the temperature from the surrounding atmosphere";
  readTemperatureCapability["endpointUrl"] = "temperature";

  return root;
}

JsonObject& createTemperatureJson() {
  DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["value"] = String(getTempInCelsius());

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