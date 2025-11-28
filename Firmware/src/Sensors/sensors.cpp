#include "sensors.h"

Sensors::Sensors(spi_host_device_t host_spi) :
//     baro(spi,systemstatus,PinMap::Baro_Cs),
    accelgyro(host_spi, PinMap::Accel_Gyro_Cs),
    accel(host_spi, PinMap::Accel_Cs)
//     mag(spi,PinMap::Mag_Cs,systemstatus),
//     logicrail("Logic Rail",PinMap::BattVolt,8,1)
{}

void Sensors::setup(){
//     baro.setup();
    accelgyro.setup();
    accel.setup();
//     mag.setup(axesOrderMMC,axesFlipMMC);
//     logicrail.setup(logicMaxVoltage,logicLowVoltage,logicMinVoltage);
    
    
};

void Sensors::update()
{
//     baro.update(sensors_raw.baro);
    accelgyro.update(sensors_raw.accelgyro);
    accel.update(sensors_raw.accel);
//     mag.update(sensors_raw.mag);
//     logicrail.update(sensors_raw.logicrail);
};

const SensorStructs::raw_measurements_t& Sensors::getData()
{
    return sensors_raw;
}
