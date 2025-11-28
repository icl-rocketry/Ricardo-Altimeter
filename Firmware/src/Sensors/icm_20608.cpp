#include "icm_20608.h"

ICM_20608::ICM_20608(spi_host_device_t host_spi, uint8_t cs):
    _host_spi(host_spi),
    _cs(cs)
{};

void ICM_20608::setup()
{
    spi_device_interface_config_t devcfg = {
        .mode = 0,                         
        .clock_speed_hz = 8 * 1000 * 1000, // 8 MHz
        .spics_io_num = _cs,    
        .queue_size = 3,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(_host_spi, &devcfg, &_spi));

    writeRegister(PWR_MGMT_1, RESET);     // reset whole device
    vTaskDelay(pdMS_TO_TICKS(100));

    writeRegister(USER_CTRL, 0x00);       // disable fifo

    writeRegister(USER_CTRL, I2C_IF_DIS); // disable I2C mode as recommended in datasheet

    writeRegister(PWR_MGMT_1, CLK_ZGYRO); // set clock source
    vTaskDelay(pdMS_TO_TICKS(5));

    //from config
    setRange(AccelRange::A_8_G,GyroRange::G_1000_DEGS); 

    writeRegister(PWR_MGMT_2,0x00); //switch everything on    
}

bool ICM_20608::alive(){
    return (readRegister(WHO_AM_I) == WHO_AM_I_RES);
}

void ICM_20608::update(SensorStructs::ACCELGYRO_6AXIS_t& data)
{
    readAccel(data.ax, data.ay, data.az);
    readGyro(data.gx, data.gy, data.gz);
    readTemp(data.temp);
}

void ICM_20608::setRange(AccelRange accel_range,GyroRange gyro_range)
{
    switch (gyro_range)
    {
    case G_250_DEGS:
        writeRegister(GYRO_CONFIG, DPS250);
        gyro_lsb_to_degs = 250.f / 32768.f;
        break;
    case G_500_DEGS:
        writeRegister(GYRO_CONFIG, DPS500);
        gyro_lsb_to_degs = 500.f / 32768.f;
        break;
    case G_1000_DEGS:
        writeRegister(GYRO_CONFIG, DPS1000);
        gyro_lsb_to_degs = 1000.f / 32768.f;
        break;
    case G_2000_DEGS:
        writeRegister(GYRO_CONFIG, DPS2000);
        gyro_lsb_to_degs = 2000.f / 32768.f;
        break;
    }

    switch (accel_range)
    {
    case A_2_G:
        writeRegister(ACCEL_CONFIG, G2);
        accel_lsb_to_g = 2.f / 32768.f;
        break;
    case A_4_G:
        writeRegister(ACCEL_CONFIG, G4);
        accel_lsb_to_g = 4.f / 32768.f;
        break;
    case A_8_G:
        writeRegister(ACCEL_CONFIG, G8);
        accel_lsb_to_g = 8.f / 32768.f;
        break;
    case A_16_G:
        writeRegister(ACCEL_CONFIG, G16);
        accel_lsb_to_g = 16.f / 32768.f;
        break;
    }
}

void ICM_20608::writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t tx[2] = { static_cast<uint8_t>(reg & 0x7F), value }; // MSB=0 for write
    spi_transaction_t t{};
    t.length    = 16;      // 2 bytes
    t.tx_buffer = tx;

    ESP_ERROR_CHECK(spi_device_transmit(_spi, &t));
}

uint8_t ICM_20608::readRegister(uint8_t reg) {
    uint8_t tx[2] = { uint8_t(reg | 0x80), 0x00 };
    uint8_t rx[2] = {0};

    spi_transaction_t t = {};
    t.length    = 16;           // total bits
    t.tx_buffer = tx;
    t.rx_buffer = rx;
    ESP_ERROR_CHECK(spi_device_transmit(_spi, &t));
    return rx[1];               // second byte is the value
}

bool ICM_20608::readGyro(float &x_dps, float &y_dps, float &z_dps)
{
    // tx[0] = address|READ, remaining dummy bytes to clock out data
    uint8_t tx[1 + 6] = { uint8_t(GYRO_XOUT_H | 0x80), 0,0,0,0,0,0 };
    uint8_t rx[1 + 6] = { 0 };

    spi_transaction_t t = {};
    t.length    = 8 * sizeof(tx);   // bits
    t.tx_buffer = tx;
    t.rx_buffer = rx;

    esp_err_t e = spi_device_transmit(_spi, &t);
    if (e != ESP_OK) {
        ESP_LOGE("ICM20608", "SPI gyro read failed: %s", esp_err_to_name(e));
        return false;
    }

    // rx[0] is junk (captured while sending addr). Data start at rx[1].
    int16_t gx = int16_t((rx[1] << 8) | rx[2]);
    int16_t gy = int16_t((rx[3] << 8) | rx[4]);
    int16_t gz = int16_t((rx[5] << 8) | rx[6]);

    x_dps = gx * gyro_lsb_to_degs;
    y_dps = gy * gyro_lsb_to_degs;
    z_dps = gz * gyro_lsb_to_degs;

    return true;
}

bool ICM_20608::readAccel(float &x_g, float &y_g, float &z_g)
{
    // tx[0] = address|READ, remaining are dummy bytes to clock out data
    uint8_t tx[1 + 6] = { uint8_t(ACCEL_XOUT_H | 0x80), 0,0,0,0,0,0 };
    uint8_t rx[1 + 6] = { 0 };

    spi_transaction_t t = {};
    t.length    = 8 * sizeof(tx);   // total bits (1 addr + 6 data)
    t.tx_buffer = tx;
    t.rx_buffer = rx;

    esp_err_t e = spi_device_transmit(_spi, &t);
    if (e != ESP_OK) {
        ESP_LOGE("ICM20608", "SPI accel read failed: %s", esp_err_to_name(e));
        return false;
    }

    // rx[0] is garbage (captured while sending the address). Data start at rx[1].
    int16_t ax = int16_t((rx[1] << 8) | rx[2]);
    int16_t ay = int16_t((rx[3] << 8) | rx[4]);
    int16_t az = int16_t((rx[5] << 8) | rx[6]);

    // Convert to g using your member/constant scale factor (g per LSB)
    // e.g. accel_lsb_to_g = 1.0f/16384 for ±2g, 1/8192 for ±4g, etc.
    x_g = ax * accel_lsb_to_g;
    y_g = ay * accel_lsb_to_g;
    z_g = az * accel_lsb_to_g;

    return true;
}

bool ICM_20608::readTemp(float &temp_degC)
{
    // Address + two dummy bytes to clock out H and L
    uint8_t tx[3] = { uint8_t(TEMP_OUT_H | 0x80), 0, 0 };
    uint8_t rx[3] = { 0 };

    spi_transaction_t t = {};
    t.length    = 8 * sizeof(tx);  // bits
    t.tx_buffer = tx;
    t.rx_buffer = rx;

    esp_err_t e = spi_device_transmit(_spi, &t);
    if (e != ESP_OK) {
        ESP_LOGE("ICM20608", "SPI temp read failed: %s", esp_err_to_name(e));
        return false;
    }

    // rx[0] is junk; data start at rx[1]
    int16_t raw = int16_t((rx[1] << 8) | rx[2]);

    float temp_offset_degC  = 25.0f;

    temp_degC = (raw / temperature_sensitivity) + temp_offset_degC;
    return true;
}