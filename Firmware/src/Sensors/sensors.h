#pragma once

#include <SPI.h>
#include <Wire.h>
#include <memory>
#include <functional>
#include <ArduinoJson.h>


#include <librrc/Helpers/jsonconfighelper.h>
#include <librnp/rnp_networkmanager.h>
#include <librnp/rnp_packet.h>


#include "Config/types.h"
#include "Config/pinmap_config.h"
#include "sensorStructs.h"



#include "dps310.h"
#include "icm_20608.h"
#include "h3lis331dl.h"
#include "mmc5983ma.h"
#include "vrailmonitor.h"

class Sensors
{
public:
    Sensors(SPIClass &spi, Types::CoreTypes::SystemStatus_t &systemstatus);

    void setup(JsonObjectConst config);
    void update();

    /**
     * @brief Get the Raw Sensor Data
     *
     * @return const SensorStructs::raw_measurements_t&
     */
    const SensorStructs::raw_measurements_t &getData();

    // Sensor Calibration Functions
    void calibrateAccelGyro();
    void calibrateHighGAccel();
    void calibrateMag(MagCalibrationParameters magcal);
    void calibrateBaro();

private:
    SensorStructs::raw_measurements_t sensors_raw;
    Types::CoreTypes::SystemStatus_t& _systemstatus;

    DPS310 baro;
    ICM_20608 accelgyro;
    H3LIS331DL accel;
    MMC5983MA mag;
    VRailMonitor logicrail;

};
