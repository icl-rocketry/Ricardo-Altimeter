#pragma once

#include "sensor_structs.h"
#include "driver/spi_master.h"


class H3LIS331DL{
    public:
        H3LIS331DL(spi_host_device_t host_spi, uint8_t cs);

        void setup();

        void update(SensorStructs::ACCEL_3AXIS_t& data);

        void startCalibrateBias();

    private:
        spi_host_device_t _host_spi;
        const uint8_t _cs;

        spi_device_handle_t _spi;

        bool alive();

        void writeRegister(uint8_t reg, uint8_t value);
        void readRegister(uint8_t reg_address, uint8_t *data);

        enum power_mode
        {
            POWER_DOWN,
            NORMAL,
            LOW_POWER_0_5HZ,
            LOW_POWER_1HZ,
            LOW_POWER_2HZ,
            LOW_POWER_5HZ,
            LOW_POWER_10HZ
        };
        enum data_rate
        {
            DR_50HZ,
            DR_100HZ,
            DR_400HZ,
            DR_1000HZ
        };

        enum int_axis
        {
            X_AXIS,
            Y_AXIS,
            Z_AXIS
        };

        enum fs_range
        {
            FS_100G,
            FS_200G,
            NO_RANGE,
            FS_400G
        };

        void setPowerMode(power_mode pmode);
        void setODR(data_rate drate);
        void axesEnable(bool enable);
        void readAxes(float &x, float &y, float &z);
        void setFullScale(fs_range range);

        float raw_to_g;    

        //registers
        static constexpr uint8_t WHO_AM_I = 0x0F;
        static constexpr uint8_t WHO_AM_I_RES = 0b00110010; 
        static constexpr uint8_t CTRL_REG1 = 0x20;
        static constexpr uint8_t CTRL_REG2 = 0x21;
        static constexpr uint8_t CTRL_REG3 = 0x22;
        static constexpr uint8_t CTRL_REG4 = 0x23;
        static constexpr uint8_t CTRL_REG5 = 0x24;
        static constexpr uint8_t HP_FILTER_RESET = 0x25;
        static constexpr uint8_t REFERENCE = 0x26;
        static constexpr uint8_t STATUS_REG = 0x27;
        static constexpr uint8_t OUT_X_L = 0x28;
        static constexpr uint8_t OUT_X_H = 0x29;
        static constexpr uint8_t OUT_Y_L = 0x2A;
        static constexpr uint8_t OUT_Y_H = 0x2B;
        static constexpr uint8_t OUT_Z_L = 0x2C;
        static constexpr uint8_t OUT_Z_H = 0x2D;
        static constexpr uint8_t INT1_CFG = 0x30;
        static constexpr uint8_t INT1_SOURCE = 0x31;
        static constexpr uint8_t INT1_THS = 0x32;
        static constexpr uint8_t INT1_DURATION = 0x33;
        static constexpr uint8_t INT2_CFG = 0x34;
        static constexpr uint8_t INT2_SOURCE = 0x35;
        static constexpr uint8_t INT2_THS = 0x36;
        static constexpr uint8_t INT2_DURATION = 0x37;
};