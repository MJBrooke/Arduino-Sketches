const int lightSensorPin = A0;

int lightSensorValue = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  //Reading can go from 0 to 1024
  lightSensorValue = analogRead(lightSensorPin);

  Serial.println(lightSensorValue);
}
