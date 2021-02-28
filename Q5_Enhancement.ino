#include <avr/sleep.h>

/*
 * Enhancement for "One Chip" Game Boy Color Q5 OSD screen kit
 * - Designed for ATmega328P QFP
 * - Adds button controls for Brightness and Palette
 * - Adds 500ms delay to Select signal to delay OSD trigger
 */

 /*
  * Idea - configure delays by holding buttons on startup
  */

/*
 * PC0 JUMPER A0
 * PC1 PALETTE_OUT A1
 * PC2 BRIGHTNESS_OUT A2
 * PC3 SELECT_OUT A3
 * PC4 A A4
 * PC5 B A5
 * PD2 SELECT 2 (int0)
 * PD3 START 3 (int1)
 */

#define JUMPER A0
#define PALETTE_OUT A1
#define BRIGHTNESS_OUT A2
#define SELECT_OUT A3
#define A A4
#define B A5
#define SELECT 2
#define START 3

#define SELECT_DELAY_MS 500
#define START_DELAY_MS 2000

const byte selectInterrupt = digitalPinToInterrupt(SELECT);
const byte startInterrupt = digitalPinToInterrupt(START);
unsigned long pressedSelectMillis = 0;
unsigned long pressedStartMillis = 0;

void setup() {
  // static inputs
  pinMode(JUMPER, INPUT_PULLUP);
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(SELECT, INPUT);
  pinMode(START, INPUT);

  // outputs (high-z when not pulled LOW)
  pinMode(PALETTE_OUT, INPUT);
  pinMode(BRIGHTNESS_OUT, INPUT);
  pinMode(SELECT_OUT, INPUT);

  delay(5);
}

void loop() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  cli();
  attachInterrupt(selectInterrupt, wakeyWakey, LOW);
  attachInterrupt(startInterrupt, wakeyWakey, LOW);
  sei();
  sleep_cpu();

  // wakes here with interrupt;

  sleep_disable();

  if (!digitalRead(START)) {
    handleBrightnessAndPalette();
  } else if (!digitalRead(SELECT)) {
    handleSelectDelay();
  }
}

void handleBrightnessAndPalette() {
  pressedStartMillis = millis();
  while(!digitalRead(START)) {
    if ((millis() - pressedStartMillis) > START_DELAY_MS) {      
      if (digitalRead(A)) {
        pinMode(PALETTE_OUT, INPUT);
      } else { // trigger palette
        digitalWrite(PALETTE_OUT, LOW);
        pinMode(PALETTE_OUT, OUTPUT);
      }

      if (digitalRead(B)) {
        pinMode(BRIGHTNESS_OUT, INPUT);
      } else { // trigger brightness
        digitalWrite(BRIGHTNESS_OUT, LOW);
        pinMode(BRIGHTNESS_OUT, OUTPUT);
      }
    }
    delay(5);
  }

  // reset
  pinMode(PALETTE_OUT, INPUT);
  pinMode(BRIGHTNESS_OUT, INPUT);
}

void handleSelectDelay() {
  pressedSelectMillis = millis();
  while (!digitalRead(SELECT)) {
    if ((millis() - pressedSelectMillis) > SELECT_DELAY_MS) {
      digitalWrite(SELECT_OUT, LOW);
      pinMode(SELECT_OUT, OUTPUT);
    }
    delay(5);
  }
  pinMode(SELECT_OUT, INPUT);
}

void wakeyWakey() {
  detachInterrupt(selectInterrupt);
  detachInterrupt(startInterrupt);
}
