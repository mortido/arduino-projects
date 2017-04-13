const int BUTTON_PIN = 4;
const int BUZZ_PIN = 5;
const int LED_PIN = 6;
const int PUMP_PIN = 13;
const int WATTERING_SECONDS = 18;

unsigned long WATTERING_DELAY = 12UL * 60UL * 60UL * 1000UL;

unsigned long timestamp = 0;
unsigned long lastwattering = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Irregation system started.");

  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(BUZZ_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT);

  timestamp = millis();
  lastwattering = timestamp;
  //Serial.println(timestamp);
  //Serial.println(WATTERING_DELAY);
}

unsigned long getDiff(unsigned long ts) {
  unsigned long result = timestamp - ts;
  if (timestamp < ts) {
    return timestamp + (4294967295 - ts);
  } else {
    return timestamp - ts;
  }
}

void loop() {
  timestamp = millis();
  //Serial.println("---");
  //Serial.println(timestamp);
  //Serial.println(lastwattering);
  //Serial.println(getDiff(lastwattering));

  if (getDiff(lastwattering) > WATTERING_DELAY
      || digitalRead(BUTTON_PIN) == LOW) {
    watter();
  }
  delay(100);
}

void watter() {
  lastwattering = timestamp;
  digitalWrite(PUMP_PIN, HIGH);
  for ( int i = 0; i < WATTERING_SECONDS; i++) {
    analogWrite(BUZZ_PIN, 50);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZ_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    delay(800);
  }
  digitalWrite(PUMP_PIN, LOW);
}

