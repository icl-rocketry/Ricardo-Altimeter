#pragma once

#include <stdint.h>
#include <type_traits>

enum class SYSTEM_FLAG:uint32_t{
    //state flags
    STATE_IDLE = (1 << 0),
    //flags
    DEBUG = (1 << 7),
    //critical messages 
    ERROR_SPI = (1 << 8),
    ERROR_I2C = (1 << 9),
    ERROR_SERIAL = (1 << 10),
    ERROR_LORA = (1 << 11),
    ERROR_BARO = (1 << 12),
    ERROR_BATT = (1 << 13),
    ERROR_IMU = (1 << 15),
    ERROR_HACCEL = (1 << 16),
    ERROR_MAG = (1 << 17),
    ERROR_ESTIMATOR = (1 << 18),
    ERROR_FLASH = (1 << 20),
    ERROR_CAN = (1 << 21),
    
};

using system_flag_t = uint32_t;

