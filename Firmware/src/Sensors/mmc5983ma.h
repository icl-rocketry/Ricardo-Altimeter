// #pragma once

// #include "sensor_structs.h"
// #include "driver/spi_master.h"

// struct MagCalibrationParameters{
//     float fieldMagnitude;
//     float inclination;
//     float declination;
//     Eigen::Matrix3f A_1;
//     Eigen::Vector3f b;
// };

// class MMC5983MA{
//     public:
//         MMC5983MA(spi_host_device_t host_spi, uint8_t cs);

//         void setup();

//         void update(SensorStructs::MAG_3AXIS_t& data);

//     private:
//         spi_host_device_t _host_spi;
//         const uint8_t _cs;

//         spi_device_handle_t _spi;

//         bool alive();

//         void writeRegister(uint8_t reg, uint8_t value);
//         void readRegister(uint8_t reg_address, uint8_t *data);

//         void readData(float &x_mag, float &y_mag, float &z_mag, float &temp);
//         void set();
//         void reset();

//         enum MBW:uint8_t{
//             MBW_100Hz,
//             MBW_200Hz,
//             MBW_400Hz,
//             MBW_800Hz
//         };

//         enum CM_Freq:uint8_t{
//             CM_OFF,
//             CM_1Hz,
//             CM_10Hz,
//             CM_20Hz,
//             CM_50Hz,
//             CM_100Hz,
//             CM_200Hz,
//             CM_1000Hz
//         };

//         enum MSET:uint8_t{
//             MSET_1,
//             MSET_25,
//             MSET_70,
//             MSET_100,
//             MSET_250,
//             MSET_500,
//             MSET_1000,
//             MSET_2000
//         };

//         static constexpr float mag_res = 1.f/16384.f; // assuming 18bit operation
//         static constexpr int32_t mag_offset = 131072;

//         static constexpr float Temp_Factor = 0.8; //0.8c/lsb
//         static constexpr float Temp_Offset = 75;

//         static constexpr uint8_t RW = 0x80;

//         //registers
//         static constexpr uint8_t WHO_AM_I = 0x2F;
//         static constexpr uint8_t WHO_AM_I_RES = 0x30; 
//         static constexpr uint8_t I2C_ADDRESS = 0x30;


//         static constexpr uint8_t XOUT_0 = 0x00;
//         static constexpr uint8_t XOUT_1 = 0x01;
//         static constexpr uint8_t YOUT_0 = 0x02;
//         static constexpr uint8_t YOUT_1 = 0x03;
//         static constexpr uint8_t ZOUT_0 = 0x04;
//         static constexpr uint8_t ZOUT_1 = 0x05;
//         static constexpr uint8_t XYZOUT_2 = 0x06;
//         static constexpr uint8_t TOUT = 0x07;
//         static constexpr uint8_t STATUS = 0x08;
//         static constexpr uint8_t CTRL_0 = 0x09;
//         static constexpr uint8_t MAG_SET = 0b00010000; // degauses internal amr
//         static constexpr uint8_t MAG_RESET = 0b00001000;
//         static constexpr uint8_t AUTO_SR = 0b00100000;

//         static constexpr uint8_t CTRL_1 = 0x0A;
//         static constexpr uint8_t RESET = 0b10000000;

//         static constexpr uint8_t CTRL_2 = 0x0B;
//         static constexpr uint8_t CM_EN = 0b00001000;
//         static constexpr uint8_t MSET_EN = 0b10000000;

//         static constexpr uint8_t CTRL_3 = 0x0C;

// };