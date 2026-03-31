#include "FastLED.h"

#define NUM_LEDS 256
#define DATA_PIN 3
#define WIDTH 16
#define HEIGHT 16
#define INTERVAL 300000 // 5 דקות בדיוק במילי-שניות

CRGB leds[NUM_LEDS];

int hour = 0;
int minuteStep = 0; // 0 עד 11 (לייצוג 0 עד 55 דקות)
unsigned long lastUpdateTime = 0; 

// הגדרות המערכים שלך (נשארות זהות)
int hours[12][3] = { {9,5,3}, {1,5,5}, {12,7,4}, {6,7,4}, {3,7,3}, {2,7,2}, {0,7,3}, {10,8,5}, {5,8,3}, {1,6,3}, {9,5,3}, {1,5,5} };
int minutes[11][3] = { {1,12,6}, {11,14,5}, {11,12,4}, {3,10,6}, {3,10,5}, {10,10,4}, {9,11,6}, {5,3,5}, {12,3,3}, {3,2,4}, {10,2,5} };

int XY(int x, int y) {
  if (x % 2 == 0) return x * 16 + y;
  else return x * 16 + (15 - y);
}

// פונקציה לכיבוי כל הלדים (פרט למילים הקבועות)
void clearWords() {
  FastLED.clear();
  // החזרת המילים הקבועות: "השעה היא אחת" (או מה שהגדרת ב-setup)
  for (int i = 12; i < 16; i++) leds[XY(i,0)] = CRGB::White; // "השעה"
  for (int i = 7; i < 10; i++) leds[XY(i,0)] = CRGB::White;  // "היא"
}

void updateDisplay() {
  clearWords();

  // הצגת השעה הנוכחית
  for (int j = 0; j < hours[hour][2]; j++) {
    leds[XY(hours[hour][0] + j, hours[hour][1])] = CRGB::White;
  }
  
  // תוספת "עשרה" לשעות הרלוונטיות (לפי הלוגיקה שלך)
  if (hour > 9) {
    for (int j = 0; j < 4; j++) leds[XY(j,6)] = CRGB::White;
  }

  // הצגת הדקות (אם אנחנו לא ב-0 דקות)
  if (minuteStep > 0) {
    int i = minuteStep - 1; // התאמה למערך הדקות שלך
    for (int j = 0; j < minutes[i][2]; j++) {
      leds[XY(minutes[i][0] + j, minutes[i][1])] = CRGB::White;
    }
    
    // לוגיקה נוספת למילים מיוחדות (כמו "וחצי" או "ורבע" לפי האינדקסים שלך)
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
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
  FastLED.setBrightness(20);
  updateDisplay();
}

void loop() {
  unsigned long currentTime = millis();

  // בדיקה אם עברו 5 דקות מהעדכון האחרון
  if (currentTime - lastUpdateTime >= INTERVAL) {
    lastUpdateTime = currentTime; 

    minuteStep++;
    
    // מעבר לשעה הבאה ב-40 דקות (לפי הלוגיקה המקורית שלך שבה i==7)
    if (minuteStep == 8) { 
      hour = (hour + 1) % 12;
    }

    // איפוס דקות אחרי שעה עגולה
    if (minuteStep > 11) {
      minuteStep = 0;
    }

    updateDisplay();
  }
}