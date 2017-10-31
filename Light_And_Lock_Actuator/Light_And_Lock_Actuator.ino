#include <ArduinoJson.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Servo.h>

BridgeServer server;

Servo servo;

const int LED_LIGHT_PIN = 3;

const int SERVO_POSITION_LOCKED = 5;
const int SERVO_POSITION_UNLOCKED = 160;

void setup() {
  Bridge.begin();
  server.begin();

  //Servo setup
  servo.attach(9);
  servo.write(SERVO_POSITION_UNLOCKED);

  //LED Light setup
  pinMode(LED_LIGHT_PIN, OUTPUT);
  digitalWrite(LED_LIGHT_PIN, LOW);
}

void loop() {
  BridgeClient client = server.accept();

  if(client.connected()) {
    processRestRequest(client);
    client.stop();
  }

  delay(50);
}

void processRestRequest(BridgeClient client) {
  String command = client.readString();
  command.trim();

  // LED
  if(command == "lightOn") {
    digitalWrite(LED_LIGHT_PIN, HIGH);
  } else if(command == "lightOff") {
    digitalWrite(LED_LIGHT_PIN, LOW);
  } 

  //Servo
  else if (command == "lock") {
    servo.write(SERVO_POSITION_LOCKED);
  } else if (command == "unlock") {
    servo.write(SERVO_POSITION_UNLOCKED);
  } 
  
  //Device
  else if (command == "deviceInformation") {
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
  root["deviceId"] = "1562651";
  root["name"] = "Light and Lock Combo";
  root["description"] = "Controls a lock mechanism and a light";
  root["endpointRoot"] = "lockandlight"; 

  JsonObject& componentType = root.createNestedObject("componentType");
  componentType["name"] = "Actuator";
  componentType["id"] = "2";

  JsonArray& capabilities = root.createNestedArray("capabilities");

  JsonObject& lightOnCapability = capabilities.createNestedObject();
  lightOnCapability["name"] = "Switch Light On";
  lightOnCapability["description"] = "Illuminates the light";
  lightOnCapability["endpointUrl"] = "lightOn";

  JsonObject& lightOffCapability = capabilities.createNestedObject();
  lightOffCapability["name"] = "Switch Light Off";
  lightOffCapability["description"] = "Deluminates the light";
  lightOffCapability["endpointUrl"] = "lightOff";

  JsonObject& lockCapability = capabilities.createNestedObject();
  lockCapability["name"] = "Lock";
  lockCapability["description"] = "Engages the lock mechanism";
  lockCapability["endpointUrl"] = "lock";

  JsonObject& unlockCapability = capabilities.createNestedObject();
  unlockCapability["name"] = "Unlock";
  unlockCapability["description"] = "Disengages the lock mechanism";
  unlockCapability["endpointUrl"] = "unlock";

  return root;
}

