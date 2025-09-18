#include "sensors.h"

Sensors::Sensors(SPIClass& spi,Types::CoreTypes::SystemStatus_t& systemstatus) :
    _systemstatus(systemstatus),
    baro(spi,systemstatus,PinMap::Baro_Cs),
    accelgyro(spi,systemstatus,PinMap::Accel_Gyro_Cs),
    accel(spi,systemstatus,PinMap::Accel_Cs),
    mag(spi,PinMap::Mag_Cs,systemstatus),
    logicrail("Logic Rail",PinMap::BattVolt,8,1)
{}

void Sensors::setup(JsonObjectConst config){
    using namespace LIBRRC::JsonConfigHelper;

    std::array<uint8_t,3> axesOrderICM{1,0,2};
    std::array<bool,3> axesFlipICM{0,1,0};

    std::array<uint8_t,3> axesOrderH3LIS{1,0,2};
    std::array<bool,3> axesFlipH3LIS{0,1,0};

    std::array<uint8_t,3> axesOrderMMC{1,0,2};
    std::array<bool,3> axesFlipMMC{0,1,0};

    uint16_t logicMaxVoltage = 4200;
    uint16_t logicLowVoltage = 3400;
    uint16_t logicMinVoltage = 3200;

    setIfContains(config,"LOGIC_MAX_VOLTAGE",logicMaxVoltage,false);
    setIfContains(config,"LOGIC_LOW_VOLTAGE",logicLowVoltage,false);
    setIfContains(config,"LOGIC_MIN_VOLTAGE",logicMinVoltage,false);

    baro.setup();
    accelgyro.setup(axesOrderICM,axesFlipICM);
    accel.setup(axesOrderH3LIS,axesFlipH3LIS);
    mag.setup(axesOrderMMC,axesFlipMMC);
    logicrail.setup(logicMaxVoltage,logicLowVoltage,logicMinVoltage);
    
    
};

void Sensors::update()
{
    baro.update(sensors_raw.baro);
    accelgyro.update(sensors_raw.accelgyro);
    accel.update(sensors_raw.accel);
    mag.update(sensors_raw.mag);
    logicrail.update(sensors_raw.logicrail);
};

const SensorStructs::raw_measurements_t& Sensors::getData()
{
    //TODO make this threadsafe maybe use a double buffer to make it lock free
    return sensors_raw;
}

void Sensors::calibrateAccelGyro()
{
    accelgyro.startCalibrateBias();
}

void Sensors::calibrateHighGAccel()
{
    accel.startCalibrateBias();
}

void Sensors::calibrateMag(MagCalibrationParameters magcal)
{
    mag.calibrate(magcal);
}

void Sensors::calibrateBaro()
{
    baro.calibrateBaro();
}

