#include <ArduinoJson.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Servo.h>

BridgeServer server;

Servo servo;
int currentServoAngle = 0;
bool increasing = true;
bool isRotating = false;

unsigned long previousTime = 0;
long interval = 500;

void setup() {
  Bridge.begin();
  server.begin();
  
  servo.attach(9);
}

void loop() {
  BridgeClient client = server.accept();

  if(client.connected()) {
    processRestRequest(client);
    client.stop();
  }

  if (isRotating) {
    rotateServo();
  }

  delay(50);
}

void processRestRequest(BridgeClient client) {
  String command = client.readString();
  command.trim();

  if (command == "start") {
    isRotating = true;
  } else if (command == "stop") {
    isRotating = false;
  } else if (command == "deviceInformation") {
    writeResponse(client, createDeviceInformationJson());
  }
}

void rotateServo() {
  unsigned long currentTime = millis();

  if(currentTime - previousTime > interval) {
    previousTime = currentTime;

    //Maxing out my servo to 0 or 179 causes a really bad crunching noise. Hence, we alternate between 5 and 160 degrees.
    currentServoAngle = (currentServoAngle == 5) ? 160 : 5; 
    servo.write(currentServoAngle);
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
  root["name"] = "Aircon";
  root["description"] = "Cools or heats up the room";
  root["endpointRoot"] = "actuator"; 

  JsonObject& componentType = root.createNestedObject("componentType");
  componentType["name"] = "Actuator";
  componentType["id"] = "2";

  JsonArray& capabilities = root.createNestedArray("capabilities");

  JsonObject& switchOnCapability = capabilities.createNestedObject();
  switchOnCapability["name"] = "Switch On";
  switchOnCapability["description"] = "Switches on the aircon unit";
  switchOnCapability["endpointUrl"] = "switchOn";

  JsonObject& switchOffCapability = capabilities.createNestedObject();
  switchOffCapability["name"] = "Switch Off";
  switchOffCapability["description"] = "Switches off the aircon unit";
  switchOffCapability["endpointUrl"] = "switchOff";

  return root;
}
