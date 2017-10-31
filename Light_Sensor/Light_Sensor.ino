#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

const int lightSensorPin = A0;

int lightSensorValue = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

BridgeServer server;

int timeUntilLcdRefresh = 0;

void setup() {
  lcd.begin(16,2);

  lcd.setCursor(6,1);
  lcd.print("Setting Up");

  Bridge.begin();
  
  server.begin();

  lcd.setCursor(10,0);
  lcd.print("Lumens");
}

void loop() {
  BridgeClient client = server.accept();

  if(client.connected()) {
    processRestRequest(client);
    client.stop();
  }

  if(timeUntilLcdRefresh == 0) {
    refreshLcdReading();
    timeUntilLcdRefresh = 1000;
  }

  timeUntilLcdRefresh -= 50;
  delay(50);
} 

void processRestRequest(BridgeClient client) {
  String command = client.readString();
  command.trim();

  if (command == "light") {
    writeResponse(client, createLightSensorResponse());
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
  root["deviceId"] = "54826";
  root["name"] = "Light Sensor";
  root["description"] = "Reads the intensity of light the surrounding the device";
  root["endpointRoot"] = "arduino";
    
  JsonObject& componentType = root.createNestedObject("componentType");
  componentType["name"] = "Sensor";
  componentType["id"] = "1";

  JsonArray& capabilities = root.createNestedArray("capabilities");

  JsonObject& readLightIntensity = capabilities.createNestedObject();

  readLightIntensity["name"] = "Read Light Intensity";
  readLightIntensity["description"] = "Reads the intensity of light the surrounding the device";
  readLightIntensity["endpointUrl"] = "light";

  return root;
}

JsonObject& createLightSensorResponse() {
  DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["value"] = String(getLightSensorReading());

  return root;
}

float getLightSensorReading() {
  return analogRead(lightSensorPin);
}

void refreshLcdReading() {
  lcd.setCursor(0,1);
  lcd.print("          ");
  lcd.print(getLightSensorReading());
}
