// #include "mmc5983ma.h"


// MMC5983MA::MMC5983MA(spi_host_device_t host_spi, uint8_t cs) : 
// _host_spi(host_spi),
// _cs(cs)
// {};

// void MMC5983MA::setup()
// {
//         spi_device_interface_config_t devcfg = {
//         .mode = 0,                       
//         .clock_speed_hz = 10 * 1000 * 1000, // 10 MHz
//         .spics_io_num = _cs,    
//         .queue_size = 3,
//     };
//     ESP_ERROR_CHECK(spi_bus_add_device(_host_spi, &devcfg, &_spi));

//     // reset chip
//     writeRegister(CTRL_1, RESET);
//     set(); // reset coils
//     reset();
//     // set control registers
//     //enable auto_set_reset
//     writeRegister(CTRL_0,AUTO_SR);
//     //set bandwidth
//     writeRegister(CTRL_1,MBW_100Hz);
//     //enable continous measurement and enable periodic set/reset
//     writeRegister(CTRL_2, CM_100Hz | 0x08 | (MSET_1000 << 4) | 0x80);
//     vTaskDelay(pdMS_TO_TICKS(100));

// }

// void MMC5983MA::update(SensorStructs::MAG_3AXIS_t& data)
// {

//     readData(data.mx, data.my, data.mz, data.temp);

// };

// bool MMC5983MA::alive(){
//     uint8_t who_am_i;
//     readRegister(WHO_AM_I, &who_am_i);
//     return (who_am_i == WHO_AM_I_RES);
// }


// void MMC5983MA::readData(float &x, float &y, float &z, float &t)
// {
//     uint32_t raw_x;
//     uint32_t raw_y;
//     uint32_t raw_z;
//     uint8_t raw_temp;

//     readRawData(raw_x,raw_y,raw_z,raw_temp);

//     x = (float)((int32_t)raw_x - mag_offset) * mag_res;
//     y = (float)((int32_t)raw_y - mag_offset) * mag_res;
//     z = (float)((int32_t)raw_z - mag_offset) * mag_res;

//     t = ((float)raw_temp * Temp_Factor) - Temp_Offset;
// }

// void MMC5983MA::readRawData(uint32_t &x, uint32_t &y, uint32_t &z, uint8_t &t)
// {
//     uint8_t rawData[8];  // x/y/z/t mag register data stored here
//     readRegister(XOUT_0
//         , &rawData[0],8);  // Read the 8 raw data registers into data array
//     x = (uint32_t)(rawData[0] << 10 | rawData[1] << 2 | (rawData[6] & 0xC0) >> 6); // Turn the 18 bits into a unsigned 32-bit value
//     y = (uint32_t)(rawData[2] << 10 | rawData[3] << 2 | (rawData[6] & 0x30) >> 4); // Turn the 18 bits into a unsigned 32-bit value
//     z = (uint32_t)(rawData[4] << 10 | rawData[5] << 2 | (rawData[6] & 0x0C) >> 2); // Turn the 18 bits into a unsigned 32-bit value
//     t = rawData[7];
// }

// void MMC5983MA::writeRegister(uint8_t reg_address, uint8_t value)
// {
//     const uint8_t addr = static_cast<uint8_t>(reg_address & 0x7F); // ensure MSB=0

//     spi_transaction_t t{};
//     t.flags    = SPI_TRANS_USE_TXDATA; // use inline 4-byte buffer (fast, no heap)
//     t.length   = 16;                   // 2 bytes total
//     t.tx_data[0] = addr;               // register address
//     t.tx_data[1] = value;              // data byte

//     ESP_ERROR_CHECK(spi_device_transmit(_spi, &t));
// }

// void MMC5983MA::set()
// {
//     writeRegister(CTRL_0,MAG_SET);
//     vTaskDelay(pdMS_TO_TICKS(5));
// }

// void MMC5983MA::reset()
// {
//     writeRegister(CTRL_0,MAG_RESET);
//     vTaskDelay(pdMS_TO_TICKS(5));
// }

// void MMC5983MA::readRegister(uint8_t reg_address, uint8_t* data)
// {
//     if (!data) return;

//     // Bit7 = 1 for read. No auto-increment needed for 1 byte.
//     const uint8_t addr = static_cast<uint8_t>((reg_address & 0x7F) | 0x80);

//     spi_transaction_t t{};
//     t.flags    = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
//     t.length   = 16;        // 2 bytes total
//     t.tx_data[0] = addr;    // address phase
//     t.tx_data[1] = 0x00;    // dummy to clock in data

//     ESP_ERROR_CHECK(spi_device_transmit(_spi, &t));

//     *data = t.rx_data[1];   // rx_data[0] is junk captured during address
// }
