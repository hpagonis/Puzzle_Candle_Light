#include <Arduino.h>
#define CANDLE_NUM 5
#define SIG_PIN 2
#define THRESHOLD 800
#define TIMEOUT 10000

struct Candle {
  uint8_t LDR_pin;
  uint8_t LED_pin;
  int threshold;
  bool lit;
};

Candle candle[CANDLE_NUM] = {
  {.LDR_pin = A0, .LED_pin = 7, .threshold = THRESHOLD, false},
  {.LDR_pin = A1, .LED_pin = 6, .threshold = THRESHOLD, false},
  {.LDR_pin = A2, .LED_pin = 5, .threshold = THRESHOLD, false},
  {.LDR_pin = A3, .LED_pin = 4, .threshold = THRESHOLD, false},
  {.LDR_pin = A4, .LED_pin = 3, .threshold = THRESHOLD, false},
};

uint8_t target_seq[5] = {2,0,3,4,1};
uint8_t player_seq[5];
uint8_t index = 0;
unsigned long last_lit = 0;


void setup() {
  for (int i=0; i < CANDLE_NUM; i++) {
    pinMode(candle[i].LED_pin, OUTPUT);
    digitalWrite(candle[i].LED_pin, LOW);
  }
  pinMode(SIG_PIN, OUTPUT);
  digitalWrite(SIG_PIN, LOW);

  // Candle light test sequence
  for (int i=0; i < CANDLE_NUM; i++) {
    digitalWrite(candle[i].LED_pin, HIGH);
    delay(200);
  }
  for (int i=0; i < CANDLE_NUM; i++) {
    digitalWrite(candle[i].LED_pin, LOW);
    delay(200);
  }

  Serial.begin(115200);
  Serial.println("Candle Light Puzzle initialized...");
}

void loop() {
  for (int i=0; i < CANDLE_NUM; i++) {
    if ((!candle[i].lit) && (analogRead(candle[i].LDR_pin) > candle[i].threshold)) {
      digitalWrite(candle[i].LED_pin, HIGH);
      candle[i].lit = true;
      player_seq[index] = i;
      index++;
      last_lit = millis();
    }
  }
  if (index == 5) {
    if (memcmp(target_seq, player_seq, 5) == 0) {
      Serial.println("Sequence input correct");
      digitalWrite(SIG_PIN, HIGH);
      delay(5000);
      digitalWrite(SIG_PIN, LOW);
    } else {
      Serial.println("Sequence input incorrect");
    }
    for (int i=0; i < CANDLE_NUM; i++) {
      digitalWrite(candle[i].LED_pin, LOW);
      candle[i].lit = false;
      delay(200);
    }
    index = 0;
    return;
  }
  if (millis() > last_lit + TIMEOUT) {
    for (int i=0; i < CANDLE_NUM; i++) {
      digitalWrite(candle[i].LED_pin, LOW);
      candle[i].lit = false;
    }
    index = 0;
  }
}