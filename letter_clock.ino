#include "FastLED.h"

#define NUM_LEDS 256
#define DATA_PIN 3
#define BUTTON_PIN 2
#define INTERVAL 300000 

CRGB leds[NUM_LEDS];

int hour = 0;
int minuteStep = 0; 
unsigned long lastUpdateTime = 0; 

// Button state variables
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; 

int hours[12][3] = { {9,5,3}, {1,5,5}, {12,7,4}, {6,7,4}, {3,7,3}, {2,7,2}, {0,7,3}, {10,8,5}, {5,8,3}, {1,6,3}, {9,5,3}, {1,5,5} };
int minutes[11][3] = { {1,12,6}, {11,14,5}, {11,12,4}, {3,10,6}, {3,10,5}, {10,10,4}, {9,11,6}, {5,3,5}, {12,3,3}, {3,2,4}, {10,2,5} };

int XY(int x, int y) {
  if (x % 2 == 0) return x * 16 + y;
  else return x * 16 + (15 - y);
}

void clearWords() {
  FastLED.clear();
  for (int i = 12; i < 16; i++) leds[XY(i,0)] = CRGB::White; 
  for (int i = 7; i < 10; i++) leds[XY(i,0)] = CRGB::White;  
}

void advanceTime() {
  minuteStep++;
  if (minuteStep == 8) { 
    hour = (hour + 1) % 12;
  }
  if (minuteStep > 11) {
    minuteStep = 0;
  }
  updateDisplay();
}

void updateDisplay() {
  clearWords();
  for (int j = 0; j < hours[hour][2]; j++) {
    leds[XY(hours[hour][0] + j, hours[hour][1])] = CRGB::White;
  }
  if (hour > 9) {
    for (int j = 0; j < 4; j++) leds[XY(j,6)] = CRGB::White;
  }
  if (minuteStep > 0) {
    int i = minuteStep - 1;
    for (int j = 0; j < minutes[i][2]; j++) {
      leds[XY(minutes[i][0] + j, minutes[i][1])] = CRGB::White;
    }
    if (i == 4 || i == 6) {
      for (int k = 4; k < 8; k++) leds[XY(k,14)] = CRGB::White;
    }
    if (i > 6) {
      for (int l = 13; l < 15; l++) leds[XY(l,5)] = CRGB::White;
    }
  }
  FastLED.show();
}

void setup() {
  Serial.begin(9600);                // <--- Start Serial communication
  Serial.println("System Online");
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
  FastLED.setBrightness(20);
  updateDisplay();
}

void loop() {
  unsigned long currentTime = millis();

  // 1. Automatic Timer
  if (currentTime - lastUpdateTime >= INTERVAL) {
    lastUpdateTime = currentTime; 
    advanceTime();
  }

  // 2. Fixed Button Logic
  int reading = digitalRead(BUTTON_PIN);

  // If the button state changed (due to noise or pressing)
  if (reading != lastButtonState) {
    lastDebounceTime = currentTime; // Reset the timer
  }

  // Only act if the state has been stable for longer than the debounce delay
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    // If the button IS pressed (LOW) and we haven't handled this press yet
    static bool isHandled = false;
    
    if (reading == LOW && !isHandled) {
      Serial.println("test"); 
      advanceTime();
      lastUpdateTime = currentTime; 
      isHandled = true; // Mark as handled so it doesn't repeat
    } 
    // Reset the handler when the button is released (HIGH)
    else if (reading == HIGH) {
      isHandled = false;
    }
  }

  lastButtonState = reading; // Save the reading for next time
}
