#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include "esp_log.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Sensors/sensor_structs.h"

class ICM_20608{
    public:
        ICM_20608(spi_host_device_t host_spi, uint8_t cs);

        void setup();

        void update(SensorStructs::ACCELGYRO_6AXIS_t& data);

    private:

        spi_host_device_t _host_spi;
        const uint8_t _cs;

        spi_device_handle_t _spi;

        enum GyroRange:uint8_t
        {
            G_250_DEGS,
            G_500_DEGS,
            G_1000_DEGS,
            G_2000_DEGS
        };

        enum AccelRange:uint8_t
        {
            A_2_G,
            A_4_G,
            A_8_G,
            A_16_G
        };

        void setRange(AccelRange accel_range,GyroRange gyro_range);

        void writeRegister(uint8_t reg, uint8_t val);

        uint8_t readRegister(uint8_t reg);

        bool readGyro(float &x_dps, float &y_dps, float &z_dps);
        bool readAccel(float &x_g, float &y_g, float &z_g);
        bool readTemp(float &temp_degC);


        bool alive();

        float gyro_lsb_to_degs;
        float accel_lsb_to_g;

        static constexpr float temperature_sensitivity = 326.8f;

        //registers
        static constexpr uint8_t WHO_AM_I = 0x75;
        static constexpr uint8_t WHO_AM_I_RES = 0xAF; //this changes dependign on the chip

        static constexpr uint8_t CONFIG = 0x1A;


        static constexpr uint8_t GYRO_CONFIG = 0x1B;
        static constexpr uint8_t DPS250 = 0b00000000;
        static constexpr uint8_t DPS500 = 0b00001000;
        static constexpr uint8_t DPS1000 = 0b00010000;
        static constexpr uint8_t DPS2000 = 0b00011000;
        // static constexpr uint8_t FCHOICE_B = 0b00000000;

        static constexpr uint8_t ACCEL_CONFIG = 0x1C;
        static constexpr uint8_t G2 = 0b00000000;
        static constexpr uint8_t G4 = 0b00001000;
        static constexpr uint8_t G8 = 0b00010000;
        static constexpr uint8_t G16 = 0b00011000;

        static constexpr uint8_t USER_CTRL = 0x6A;
        static constexpr uint8_t I2C_IF_DIS = 0b00010000;
        static constexpr uint8_t FIFO_EN = 0b01000000;

        static constexpr uint8_t PWR_MGMT_1 = 0x6B;
        static constexpr uint8_t PWR_MGMT_2 = 0x6C;
        static constexpr uint8_t SLEEP = 0b01000000;
        static constexpr uint8_t RESET = 0b10000000;
        static constexpr uint8_t CLK_ZGYRO = 0b00000011;


        static constexpr uint8_t ACCEL_XOUT_H = 0x3B;
        static constexpr uint8_t ACCEL_XOUT_L = 0x3C;
        static constexpr uint8_t ACCEL_YOUT_H = 0x3D;
        static constexpr uint8_t ACCEL_YOUT_L = 0x3E;
        static constexpr uint8_t ACCEL_ZOUT_H = 0x3F;
        static constexpr uint8_t ACCEL_ZOUT_L = 0x40;

        static constexpr uint8_t TEMP_OUT_H = 0x41;
        static constexpr uint8_t TEMP_OUT_L = 0x42;

        static constexpr uint8_t GYRO_XOUT_H = 0x43;
        static constexpr uint8_t GYRO_XOUT_L = 0x44;
        static constexpr uint8_t GYRO_YOUT_H = 0x45;
        static constexpr uint8_t GYRO_YOUT_L = 0x46;
        static constexpr uint8_t GYRO_ZOUT_H = 0x47;
        static constexpr uint8_t GYRO_ZOUT_L = 0x48;

};