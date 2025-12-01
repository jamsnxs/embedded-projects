constexpr int PIN_BUZZER{ 14 };  // GPIO14 (D5)
constexpr int PIN_TRIG{ 13 };    // GPIO13 (D7)
constexpr int PIN_ECHO{ 12 };    // GPIO12 (D6)

constexpr float DANGER_DIST_CM{ 30.0f };
constexpr float ALERT_DIST_CM{ 60.0f };
constexpr int DANGER_FREQ_HZ{ 1000 };
constexpr int ALERT_FREQ_HZ{ 600 };

float measureDistanceCm();

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_TRIG, LOW);  // Ensure TRIG is LOW for clean start
  Serial.begin(115200);
}

float measureDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);  // Protocol requirement
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long durationUs = pulseIn(PIN_ECHO, HIGH);

  // Convert echo time to distance (cm)
  return (durationUs * 0.0343f) / 2.0f;
}

void loop() {
  float distanceCm = measureDistanceCm();

  unsigned long now = millis();
  static unsigned long lastLogTimeMs{};

  // Logging (Non-blocking): Log every 1000 ms
  if (now - lastLogTimeMs >= 1000) {
    Serial.print("Distance: ");
    Serial.print(distanceCm, 2);
    Serial.println(" cm");
    lastLogTimeMs = now;
  }

  // Buzzer Alert Logic (Non-blocking State Machine)
  static int alertFrequency{};
  static bool buzzerIsOn{};

  if (distanceCm > 0.0f && distanceCm <= DANGER_DIST_CM) {
    alertFrequency = DANGER_FREQ_HZ;
  } else if (distanceCm > DANGER_DIST_CM && distanceCm <= ALERT_DIST_CM) {
    alertFrequency = ALERT_FREQ_HZ;
  } else {
    // Safe Zone
    if (buzzerIsOn) {
      noTone(PIN_BUZZER);
      buzzerIsOn = false;
    }
    return;
  }

  static unsigned long lastBuzzerToggleTime{};
  // Non-blocking Toggle Logic: Check if 200 ms has elapsed
  if (now - lastBuzzerToggleTime >= 200) {

    if (buzzerIsOn) {
      // Turn OFF
      noTone(PIN_BUZZER);
      buzzerIsOn = false;
    } else {
      // Turn ON with the determined frequency
      tone(PIN_BUZZER, alertFrequency);
      buzzerIsOn = true;
    }

    lastBuzzerToggleTime = now;
  }
}
