#include "h3lis331dl.h"



H3LIS331DL::H3LIS331DL(spi_host_device_t host_spi, uint8_t cs):
_host_spi(host_spi),
_cs(cs)
{};

void H3LIS331DL::setup()
{
    spi_device_interface_config_t devcfg = {
        .mode = 0,                         // SPI mode 0
        .clock_speed_hz = 10 * 1000 * 1000, // 10 MHz
        .spics_io_num = _cs,    // your CS pin for the IMU
        .queue_size = 3,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(_host_spi, &devcfg, &_spi));

    setPowerMode(power_mode::NORMAL);
    axesEnable(true);

    uint8_t data = 0;
    for (int i = 0x21; i < 0x25; i++) writeRegister(i,data);
    for (int i = 0x30; i < 0x37; i++) writeRegister(i,data);

    setODR(data_rate::DR_400HZ);
    setFullScale(fs_range::FS_100G); 

}

void H3LIS331DL::update(SensorStructs::ACCEL_3AXIS_t& data)
{
    readAxes(data.ax,data.ay,data.az);
};

bool H3LIS331DL::alive(){
    uint8_t data;
    readRegister(WHO_AM_I, &data);
    return (data == WHO_AM_I_RES);
}

void H3LIS331DL::axesEnable(bool enable)
{
    uint8_t data;
    readRegister(CTRL_REG1, &data);
    if (enable)
    {
        data |= 0x07;
    }
    else
    {
        data &= ~0x07;
    }
    writeRegister(CTRL_REG1, data);
}

void H3LIS331DL::setPowerMode(power_mode pmode)
{
    uint8_t data;
    readRegister(CTRL_REG1, &data);

    // The power mode is the high three bits of CTRL_REG1. The mode 
    //  constants are the appropriate bit values left shifted by five, so we 
    //  need to right shift them to make them work. We also want to mask off the
    //  top three bits to zero, and leave the others untouched, so we *only*
    //  affect the power mode bits.
    data &= ~0xe0; // Clear the top three bits
    data |= pmode<<5; // set the top three bits to our pmode value
    writeRegister(CTRL_REG1, data); // write the new value to CTRL_REG1
}

void H3LIS331DL::setODR(data_rate drate)
{
    uint8_t data;
    readRegister(CTRL_REG1, &data);

    // The data rate is bits 4:3 of CTRL_REG1. The data rate constants are the
    //  appropriate bit values; we need to right shift them by 3 to align them
    //  with the appropriate bits in the register. We also want to mask off the
    //  top three and bottom three bits, as those are unrelated to data rate and
    //  we want to only change the data rate.
    data &=~0x18;     // Clear the two data rate bits
    data |= drate<<3; // Set the two data rate bits appropriately.
    writeRegister(CTRL_REG1, data); // write the new value to CTRL_REG1
}

void H3LIS331DL::setFullScale(fs_range range)
{
    uint8_t data;
    readRegister(CTRL_REG4, &data);
    data &= ~0xcf;
    data |= range << 4;
    writeRegister(CTRL_REG4, data);
    switch (range)
    {
    case fs_range::FS_100G:
    {
        raw_to_g = 100.0f / 2047.0f;
        break;
    }
    case fs_range::FS_200G:
    {
        raw_to_g = 200.0f / 2047.0f;
        break;
    }
    case fs_range::FS_400G:
    {
        raw_to_g = 400.0f / 2047.0f;
        break;
    }
        default:
        {
            raw_to_g = 0;
            // log error
            break;
        }
    }
}

void H3LIS331DL::readAxes(float &x_g, float &y_g, float &z_g)
{
  uint8_t data[6]; // create a buffer for our incoming data
  readRegister(OUT_X_L, &data[0]);
  readRegister(OUT_X_H, &data[1]);
  readRegister(OUT_Y_L, &data[2]);
  readRegister(OUT_Y_H, &data[3]);
  readRegister(OUT_Z_L, &data[4]);
  readRegister(OUT_Z_H, &data[5]);
  // The data that comes out is 12-bit data, left justified, so the lower
  //  four bits of the data are always zero. We need to right shift by four,
  //  then typecase the upper data to an integer type so it does a signed
  //  right shift.
  uint16_t x = data[0] | data[1] << 8;
  uint16_t y = data[2] | data[3] << 8;
  uint16_t z = data[4] | data[5] << 8;
  x = x >> 4;
  y = y >> 4;
  z = z >> 4;
  x_g = raw_to_g * (float)(x);
  y_g = raw_to_g * (float)(y);
  z_g = raw_to_g * (float)(z);
}

// Single-byte write: same shape as your ICM function
void H3LIS331DL::writeRegister(uint8_t reg, uint8_t value)
{
    // For LIS parts: MSB=0 => write. Auto-increment not needed for 1 byte.
    uint8_t tx[2] = { static_cast<uint8_t>(reg & 0x7F), value };

    spi_transaction_t t{};
    t.length    = 16;          // 2 bytes
    t.tx_buffer = tx;

    ESP_ERROR_CHECK(spi_device_transmit(_spi, &t));
}

void H3LIS331DL::readRegister(uint8_t reg_address, uint8_t* data)
{
    if (!data) return;

    // Bit7=1 (read). No auto-increment for 1 byte.
    const uint8_t addr = static_cast<uint8_t>((reg_address & 0x7F) | 0x80);

    spi_transaction_t t{};
    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.length = 16;                 // 2 bytes total
    t.tx_data[0] = addr;           // send address
    t.tx_data[1] = 0x00;           // dummy to clock in the data

    ESP_ERROR_CHECK(spi_device_transmit(_spi, &t));

    *data = t.rx_data[1];          // rx_data[0] is junk during addr phase
}