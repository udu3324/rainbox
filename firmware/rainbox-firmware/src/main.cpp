#include <Arduino.h>
#include <FastLED.h>

// neopixels
#define NEOPIXEL_BUILTIN  18
#define NUM_LEDS_BUILTIN  2

CRGB ledsB[NUM_LEDS_BUILTIN];

#define NEOPIXEL_STRIP    8
#define NUM_LEDS_STRIP    1 // change if you have one plugged in

CRGB ledsS[NUM_LEDS_STRIP];

// mist sensor
#define MIST_SENSOR       9

// water sensors
#define WATER_SENSOR_1    14
#define WATER_SENSOR_2    2

// water pump controller
#define WATER_EN_IN1      4
#define WATER_PH_IN2      5    
#define WATER_SLEEP_N     6

// stepper controller
#define PLANE_DIR         38
#define PLANE_STEP        39
#define PLANE_ENABLE      40
#define PLANE_MOSI        10
#define PLANE_SPI_CLK     11
#define PLANE_CS          12
#define PLANE_MISO        13

boolean waterPlaneEnabled = true;

void setup() {
  FastLED.addLeds<NEOPIXEL, NEOPIXEL_BUILTIN>(ledsB, NUM_LEDS_BUILTIN);
  FastLED.addLeds<NEOPIXEL, NEOPIXEL_STRIP>(ledsS, NUM_LEDS_STRIP);

  pinMode(PLANE_DIR, OUTPUT);
  pinMode(PLANE_STEP, OUTPUT);
  pinMode(PLANE_ENABLE, OUTPUT);
}

void loop() {
  
}

void turnOnWaterPlane(boolean on) { // in prod: check direction + time needed
  if (on && !waterPlaneEnabled) {
    digitalWrite(PLANE_DIR, HIGH);
    digitalWrite(PLANE_STEP, HIGH);
  } else if (!on && waterPlaneEnabled) {
    digitalWrite(PLANE_DIR, LOW);
    digitalWrite(PLANE_STEP, HIGH);
  }

  delayMicroseconds(500);

  // turn off
  digitalWrite(PLANE_STEP, LOW);

  waterPlaneEnabled = on;
}