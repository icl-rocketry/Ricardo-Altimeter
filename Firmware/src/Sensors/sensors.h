// #pragma once

// #include <memory>
// #include <functional>

#include "Config/pinmap_config.h"
#include "sensor_structs.h"



#include "icm_20608.h"
#include "h3lis331dl.h"
// #include "mmc5983ma.h"
// #include "adc_vrailmonitor.h"

class Sensors
{
public:
    Sensors(spi_host_device_t host_spi);

    void setup();
    void update();

    const SensorStructs::raw_measurements_t &getData();


private:
    SensorStructs::raw_measurements_t sensors_raw;

    static constexpr char TAG[] = "SENSORS";


//     DPS310 baro;
    ICM_20608 accelgyro;
    H3LIS331DL accel;
//     MMC5983MA mag;
//     ADC_VRailMonitor logicrail;

};
