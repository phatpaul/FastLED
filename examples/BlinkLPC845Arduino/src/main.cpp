/// @file    Blink_LPC845.cpp
/// @brief   FastLED test for LPC845 - WS2812 strip blink
/// @example Blink_LPC845.cpp

// FL_AGENT_ALLOW_NEW_EXAMPLE
#include <Arduino.h>
#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 2

// GPIO pin for LED data line (adjust for your hardware)
#define PIN_DATA 0

// Define the array of leds
CRGB leds[NUM_LEDS];

// Setup function called once at startup
extern "C" void setup() {
    Serial.begin(115200);
    Serial.println("\n==============================================");
    Serial.println("   BLINK.INO - Simple LED Blink Example");
    Serial.println("==============================================\n");

    // Initialize FastLED - use WS2812/NEOPIXEL protocol
    FastLED.addLeds<NEOPIXEL, PIN_DATA>(leds, NUM_LEDS);
}

// Loop function called repeatedly
extern "C" void loop() {
    // Turn the LED on (red)
    leds[0] = CRGB::Red;
    FastLED.show();
    fl::delay(100);

    Serial.println(".");

    // Turn the LED off
    leds[0] = CRGB::Black;
    FastLED.show();
    fl::delay(100);
}
