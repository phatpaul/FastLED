#include <FastLED.h>
#include <stdint.h>

#define LED_PIN 0
#define NUM_LEDS 4
#define BRIGHTNESS 255
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

void wait_for_serial_connection() {
    uint32_t timeout_end = millis() + 2000;
    Serial.begin(115200);
    while (!Serial && timeout_end > millis()) {
    } // wait until the connection to the PC is established
}

void nextColor() {
    static uint8_t colorIndex = 0;
    colorIndex = (colorIndex + 1) % 3; // Cycle through 3 colors

    for (int i = 0; i < NUM_LEDS; ++i) {
        switch (colorIndex) {
            case 0:
                leds[i] = CRGB::Red;
                break;
            case 1:
                leds[i] = CRGB::Green;
                break;
            case 2:
                leds[i] = CRGB::Blue;
                break;
        }
    }
}

void setup() {
    wait_for_serial_connection(); // Optional, but seems to help Teensy out a
                                  // lot.
    Serial.println("   BLINK.INO - Simple LED Blink Example");

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);//.setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    nextColor(); // Initialize the first color
}


void loop() {

    EVERY_N_SECONDS( 1 ) { nextColor(); } // change color periodically

    FastLED.show();
}
