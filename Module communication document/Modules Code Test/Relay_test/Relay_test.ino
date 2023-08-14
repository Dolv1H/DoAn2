int relayPin = 23;

void setup() {
  pinMode(relayPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readString();
    if (cmd == "on") {
      digitalWrite(relayPin, HIGH);
      Serial.println("ON");
    } else if (cmd == "off") {
      digitalWrite(relayPin, LOW);
      Serial.println("OFF");
    }
  }
}