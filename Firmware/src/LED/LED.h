#pragma once

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Config/pinmap_config.h"

class RGBLed {
public:
    RGBLed()
        : activeLow(true)
    {
        gpio_config_t io_conf = {};
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask =
            (1ULL << PinMap::LED_RED) |
            (1ULL << PinMap::LED_GREEN) |
            (1ULL << PinMap::LED_BLUE);
        gpio_config(&io_conf);

        off(); // all off at start
    }

    // Solid colours
    void red()   { set(true, false, false); }
    void green() { set(false, true, false); }
    void blue()  { set(false, false, true); }

    void yellow()  { set(true, true, false); }
    void cyan()    { set(false, true, true); }
    void magenta() { set(true, false, true); }
    void white()   { set(true, true, true); }

    void off()     { set(false, false, false); }

private:
    bool activeLow;

    void set(bool r, bool g, bool b)
    {
        // If LED is wired active-low, flip the logic
        int rv = activeLow ? !r : r;
        int gv = activeLow ? !g : g;
        int bv = activeLow ? !b : b;

        gpio_set_level(PinMap::LED_RED, rv);
        gpio_set_level(PinMap::LED_GREEN, gv);
        gpio_set_level(PinMap::LED_BLUE, bv);
    }
};
