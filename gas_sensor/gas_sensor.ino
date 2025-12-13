constexpr int sensorPin = A0;
constexpr int ledPin = D6;
constexpr int buzzerPin = D5;

unsigned long previousTime{};

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int reading = analogRead(sensorPin);
  int percentage = (reading * 100) / 1023;

  if (percentage >= 25) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    tone(buzzerPin, 4000);
    delay(100);
    noTone(buzzerPin);
    delay(100);
  } else {
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }

  unsigned long now = millis();
  if (now - previousTime >= 300) {
    Serial.print("Gas concentration: ");
    Serial.print(percentage);
    Serial.println("%");
    previousTime = now;
  }
}
