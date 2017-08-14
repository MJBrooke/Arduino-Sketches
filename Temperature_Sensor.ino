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
  lcd.print("Prototype - D06");

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
    respondWithComponentDetails(client);
  } else if (command == "temperature") {
    respondWithTemperature(client);
  }
}

void respondWithComponentDetails(BridgeClient client){
  int numValues = 4;
  String keyValuePairs[numValues] = {
    "name", "Temperature Sensor",
    "description", "Reads the temperature of the surrounding environment"
  };

  setupClientResponse(client, keyValuePairs, numValues);
}

void respondWithTemperature(BridgeClient client) {
  float tempCelsius = getTempInCelsius();

  int numValues = 2;
  String keyValuePairs[numValues] = {"temperature", String(tempCelsius)};

  setupClientResponse(client, keyValuePairs, numValues);
}

float getTempInCelsius() {
  int analogTempReading = analogRead(temperatureSensorPin);
  
  float voltage = (analogTempReading/1024.0) * 5;
  
  float tempCelsius = (voltage - .5) * 100;

  return tempCelsius;
}

void refreshLcdTempReading() {
  float tempCelsius = getTempInCelsius();
  
  lcd.setCursor(1,1);
  lcd.print("Temp: ");
  lcd.print(tempCelsius);
  lcd.print("C");
}

void setupClientResponse(BridgeClient client, String keyValuePairs[], int numValues) {
  client.println("Status: 200");
  client.println("Content-type: application/json");
  client.println();
  client.println(createJsonResponse(keyValuePairs, numValues));
}

String createJsonResponse(String keyValuePairs[], int numValues) {
  String jsonResponse = String();
  
  jsonResponse += "{";
  
  for(int i = 0; i < numValues; i = i + 2) {
    
    if(i != 0) {
      jsonResponse += ",";
    }
    
    jsonResponse += "\"" + keyValuePairs[i] + "\"";
    jsonResponse += ":";
    jsonResponse += "\"" + keyValuePairs[i+1] + "\"";
  }
  
  jsonResponse += "}";

  return jsonResponse;
}
