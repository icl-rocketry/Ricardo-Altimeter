/*
**********************
* PINS               *
**********************
 */
#pragma once

#include "driver/gpio.h"

namespace PinMap{
    static constexpr gpio_num_t LED_RED = GPIO_NUM_1;
    static constexpr gpio_num_t LED_BLUE = GPIO_NUM_2;
    static constexpr gpio_num_t LED_GREEN = GPIO_NUM_4;

    // for flash ic (using standard spi mode)
    static constexpr int IC_SCLK = 9; // 120Mhz
    static constexpr int IC_MOSI = 10; // SIO0 for quad spi
    static constexpr int IC_MISO = 12; // SIO1 for quad spi
    static constexpr int IC_Cs = 13;
    
    static constexpr int IC_WP = 11; // SIO2 for quad spi
    static constexpr int IC_Hold = 14; // SIO3 for quad spi


    static constexpr int Mag_Cs = 18;
    static constexpr int Baro_Cs = 21;
    static constexpr int Accel_Cs = 48;
    static constexpr int Accel_Gyro_Cs = 38;

    static constexpr int SNSR_MISO = 47;
    static constexpr int SNSR_MOSI = 33;
    static constexpr int SNSR_SCLK = 34;

    static constexpr int Buzzer = 40;

    static constexpr int J1_1 = 36;
    static constexpr int J1_2 = 37;

    static constexpr int BattVolt = 8;

}