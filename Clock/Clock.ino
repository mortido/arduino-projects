//#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <Time.h>

#include <Wire.h>
#include "rgb_lcd.h"

Encoder encoder(2, 3);
rgb_lcd lcd;

const int BUTTON_PIN = 4;
const int BUZZ_PIN =  5;
const long MAX_TIMER = 90 * 60; // Seconds.
const int TIMER_START_DELAY =  3;
const int TIME_SET_DELAY = 3500;

unsigned long timestamp = 0;

// TIME
time_t currentTime = 0;
int setupMode = 0;
tmElements_t cur;
unsigned long setupTimestamp = 0;

// TIMER
bool timerEnabled = false;
long timerCounter = 0;
time_t timerTimestamp = 0;

// COMMON
long oldEncoderPosition  = 0;
int displayMode = 0; // 0 -Time, 1 - Timer
unsigned long buttonTimestamp = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Clock started.");
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);  
  lcd.print("<< Test clock >>");

  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(BUZZ_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  timestamp = millis();

  if(setupMode == 0) {
      updateTime();
      updateTimer();
  }

  handleButton();
  handleEncoder();
  
  if(displayMode == 0) {
    showTime();
  } else {
    showTimer();
  }
  delay(100);
}

void handleButton(){
  if(digitalRead(BUTTON_PIN) == LOW) {
    if(buttonTimestamp == 0) {
      buttonTimestamp = timestamp;
    }

    if(setupMode == 0 && getDiff(buttonTimestamp) >= TIME_SET_DELAY)
    {
      setupMode = 1;
      timerEnabled = false;
      timerCounter = 0;
      displayMode = 0;
    }
  } else if (buttonTimestamp != 0) {
    if(setupMode == 1 && getDiff(buttonTimestamp) >= TIME_SET_DELAY)
    {
      // Do nothing.
    } else if (setupMode > 0) {
      setupMode++;
      if(setupMode == 3) {
        setupMode = 0;

        // Update RTC values.
      }
    } else if (timerCounter > 0) {
      ++displayMode%=2;
    } else if (timerEnabled) {
      timerEnabled = false;
      displayMode = 0;
      // Disable alarm.
    }
    buttonTimestamp = 0;
  }
}

int getDiff(unsigned long ts) {
  unsigned long result = timestamp - ts;
  if(timestamp < ts) {
    return timestamp + (4294967295 - ts);
  } else {
    return timestamp - ts;
  }
}

void handleEncoder() {
  long newPosition = encoder.read();
  newPosition = newPosition >> 1;
  if (newPosition != oldEncoderPosition) {
    if(setupMode == 0) {
      displayMode = 1;
      timerTimestamp = currentTime;
      timerEnabled = false;

      timerCounter += (newPosition - oldEncoderPosition) * 60;

      if(timerCounter > MAX_TIMER) {
        timerCounter = MAX_TIMER;
      } else if (timerCounter <= 0) {
        timerCounter = 0;
        displayMode = 0;
      }
    } else {
      breakTime(currentTime, cur);
      if(setupMode == 1) {
        int hhh = cur.Hour;
        hhh += newPosition - oldEncoderPosition;
        if(hhh < 0) {
          hhh += 24;
        }

        hhh %= 24;
        cur.Hour = hhh;
      } else if(setupMode == 2) {
        int mmm = cur.Minute;
        mmm += newPosition - oldEncoderPosition;
        if(mmm < 0) {
          mmm += 60;
        }

        mmm %= 60;
        cur.Minute = mmm;
      }

      setupTimestamp = timestamp;
      currentTime = makeTime(cur);
      setTime(currentTime);
    }

    oldEncoderPosition = newPosition;
  }

  if(timerCounter > 0 && !timerEnabled){
    long delta = currentTime - timerTimestamp;

    if(delta >= TIMER_START_DELAY) {
      timerEnabled = true;
      timerTimestamp = currentTime;
    }
  }
}

void updateTime() {
  currentTime = now();
}

void updateTimer() {
  if(timerEnabled) {
    if(timerCounter > 0) {
      timerCounter += timerTimestamp - currentTime;
      timerTimestamp = currentTime;
      if(timerCounter < 0) {
        timerCounter = 0;
      }
    }

    if(timerCounter == 0) {
      // TODO: allarm buzzer + led lights.
    }
  }
}

void showTime(){
  if(setupMode == 1 && (timestamp / 500) % 2 == 0 && getDiff(setupTimestamp) > 500){
    lcd.setCursor(0, 0);
    lcd.print("__");
  } else {
    showHours(hour(currentTime));
  }

  if(setupMode == 2 && (timestamp / 500) % 2 == 0 && getDiff(setupTimestamp) > 500) {
    lcd.setCursor(3, 0);
    lcd.print("__");
  } else {
    showMinutes(minute(currentTime));
  }

  updateDots(setupMode != 0);
}

void showTimer() {
  int minutes = timerCounter / 60;
  if(timerCounter > 0){
    showHours(minutes);
    showMinutes(timerCounter % 60);
    updateDots(!timerEnabled);
  } else if((timestamp / 500) % 2 == 0){
    lcd.setCursor(0, 0);
    lcd.print("00:00");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("-- --");
  }
}

void showHours(int h){
  lcd.setCursor(0, 0);
  lcd.print((h % 100) / 10);
  lcd.print(h % 10);
}

void showMinutes(int m) {
  lcd.setCursor(3, 0);
  lcd.print((m % 100) / 10);
  lcd.print(m % 10);
}

void updateDots(bool freezed) {
  lcd.setCursor(2, 0);
  if((timestamp / 500) % 2 == 0 || freezed){
    lcd.print(":");
  } else {
    lcd.print(" ");
  }
}

