/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"

// fl::platforms time (delay/millis/micros + SysTick_Handler) is compiled by the
// FastLED platforms+.cpp build unit (see CMakeLists). Do NOT also #include the
// .cpp.hpp implementation here -- that produces multiple-definition link errors.
#include <FastLED.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

namespace
{
    // PIO0_15 on the LPC845-BRK / LPCXpresso845-MAX breakout header.
    // Wire an external LED + series resistor here.
    // (FastPin only covers PORT0 today, so the on-board RGB LED on
    //  PORT1 isn't reachable yet — see fastpin_arm_lpc.h.)
    constexpr unsigned LED_PIN = 16;
    constexpr unsigned BLINK_MS = 250; // 2 Hz toggle
    // How many leds in your strip?
    constexpr unsigned NUM_LEDS = 1;
    // For led chips like WS2812, which have a data line, ground, and power, you just
    // need to define PIN_DATA.  For led chipsets that are SPI based (four wires - data, clock,
    // ground, and power), like the LPD8806 define both PIN_DATA and CLOCK_PIN
    // Clock pin only needed for SPI based chipsets when not using hardware SPI
    constexpr unsigned PIN_DATA = 0; // PIO0_0 on the LPC845-BRK / LPCXpresso845-MAX breakout header
    // Define the array of leds
    CRGB leds[NUM_LEDS];
} // namespace

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

extern "C" int main(void)
{
    /* Init board hardware. */
    BOARD_InitHardware();
    GPIO_PortInit(GPIO, 0); // needed for FastPin<16> to work, since it uses the PORT0 DIR/SET/CLR registers directly without going through the GPIO driver

    PRINTF("MCUX SDK version: %s\r\n", MCUXSDK_VERSION_FULL_STR);
    PRINTF("Clock Frequency: %u Hz\r\n", (unsigned)CLOCK_GetFreq(kCLOCK_CoreSysClk));

    FastLED.addLeds<NEOPIXEL, PIN_DATA>(leds, NUM_LEDS); // GRB ordering is assumed
    fl::FastPin<LED_PIN>::setOutput();

    PRINTF("hello world.\r\n");


    fl::u32 last_log_ms = fl::millis();

    for (;;)
    {

        // Turn the LED on, then pause
        for (unsigned i = 0; i < NUM_LEDS; ++i)
        {
            leds[i] = CRGB::Red;
        }
        FastLED.show();
        fl::FastPin<LED_PIN>::hi();
        fl::delay(BLINK_MS);

        // Now turn the LED off, then pause
        for (unsigned i = 0; i < NUM_LEDS; ++i)
        {
            leds[i] = CRGB::Black;
        }
        FastLED.show();
        fl::FastPin<LED_PIN>::lo();
        fl::delay(BLINK_MS);
        PRINTF(".");

        const fl::u32 now = fl::millis();
        if ((now - last_log_ms) >= 2000)
        {
            PRINTF("heartbeat.\r\n");
            last_log_ms = now;
        }
    }
    return 0;
}
