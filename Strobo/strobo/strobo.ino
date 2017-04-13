#include <Encoder.h>
Encoder encoder(2, 3);

const int BUTTON_PIN = 4;

const int BTN1_PIN = 9;
const int BTN2_PIN = 11;
const int BTN3_PIN = 12;

const int LED_PIN = 10;
long oldEncoderPosition  = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("System started.");
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  pinMode(BTN3_PIN, INPUT_PULLUP);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

int light_time = 200;
unsigned long rate = 40000;
unsigned long last_blink = 0;
unsigned long adjust = 0;

void loop() {
  //unsigned long current_micros2 = micros();
  handleEncoder();
  //Serial.println(micros()-current_micros2);
  
  unsigned long current_micros = micros();
  if(current_micros - last_blink >= rate + adjust){

    adjust = 0; // Reset adjustment.
    last_blink = current_micros;

    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(light_time);
    digitalWrite(LED_PIN, LOW);
  }
}

void handleEncoder() {
  long newPosition = encoder.read();
  newPosition = newPosition >> 1;
  if (newPosition != oldEncoderPosition) {

    boolean bightness_adjusment = !digitalRead(BTN1_PIN);
    boolean fine_adjusment = digitalRead(BTN2_PIN);

    if(bightness_adjusment){
      light_time += (oldEncoderPosition - newPosition) * (fine_adjusment ? 1UL : 100UL);
    }else{
      rate += (oldEncoderPosition - newPosition) * (fine_adjusment ? 1UL : 100UL);
    }

    light_time = max(200UL, min(min(50000UL, light_time), rate / 2UL));
    
    Serial.print(" Rate: ");
    Serial.print(rate);
    Serial.print(" Ligt Time: ");
    Serial.print(light_time);
    Serial.println("");
    oldEncoderPosition = newPosition;
  }
}
