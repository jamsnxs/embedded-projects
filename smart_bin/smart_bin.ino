#include <Servo.h>

constexpr int PIN_TRIG{ 9 };
constexpr int PIN_ECHO{ 10 };
constexpr int PIN_SERVO{ 11 };

constexpr int OPEN_ANGLE{ 160 };
constexpr int CLOSE_ANGLE{ 0 };
constexpr float DIST_OPEN_CM{ 25.0f };
constexpr float DIST_IGNORE_CM{ 100.0f };
constexpr unsigned long AUTO_CLOSE_MS{ 1000 };

Servo servo;

void setup() {
  servo.attach(PIN_SERVO);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  Serial.begin(115200);
}

float measure_distance_cm() {
  digitalWrite(PIN_TRIG, LOW);  // Start the HC with a clean state
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long echo = pulseIn(PIN_ECHO, HIGH);
  return (echo * 0.0343f) / 2.0f;  // convert echo time to distance (cm)
}

void loop() {
  float distance = measure_distance_cm();
  if (distance > 100.0f) return;

  unsigned long now = millis();
  static unsigned long print_timestamp_ms{};

  // Serial print for debug, no delay are add to the loop execution
  if (now - print_timestamp_ms >= 1000) {
    Serial.print("distance: ");
    Serial.print(distance);
    Serial.print(" cm  |  servo angle: ");
    Serial.println(servo.read());
    print_timestamp_ms = now;
  }

  static unsigned long last_move_ms{};

  if (distance <= DIST_OPEN_CM) {
    servo.write(OPEN_ANGLE);
    last_move_ms = millis();  // Mark the time
  } else if (millis() - last_move_ms >= AUTO_CLOSE_MS) {
    servo.write(CLOSE_ANGLE);
  }
}
