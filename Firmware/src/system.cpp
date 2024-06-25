#include "system.h"

#include <memory>

#include <ArduinoJson.h>

#include <libriccore/riccoresystem.h>
#include <libriccore/storage/wrappedfile.h>


#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"
#include "Config/services_config.h"

#include "Commands/commands.h"

#include "Network/Interfaces/radio.h"
#include <libriccore/networkinterfaces/can/canbus.h>

#include "Sensors/sensors.h"
#include "Sensors/estimator.h"
#include "Sensors/sensorStructs.h"

#include "Sound/tunezHandler.h"


#include "States/idle.h"

// #include "hal/usb_serial_jtag_ll.h"




#ifdef CONFIG_IDF_TARGET_ESP32S3
static constexpr int VSPI_BUS_NUM = 0;
static constexpr int HSPI_BUS_NUM = 1;
#else
static constexpr int VSPI_BUS_NUM = VSPI;
static constexpr int HSPI_BUS_NUM = HSPI;
#endif

System::System() : RicCoreSystem(Commands::command_map, Commands::defaultEnabledCommands, Serial),
                   hspi(HSPI_BUS_NUM),
                   sensors(hspi, systemstatus),
                   estimator(systemstatus)
                   {};

void System::systemSetup()
{

    Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
    Serial.begin(GeneralConfig::SerialBaud);
  
    statemachine.initalize(std::make_unique<Idle>(systemstatus,commandhandler));
    setupPins();
    // initalize spi interface
    setupSPI();

    tunezhandler.setup();

    // add interfaces to netmanager
    configureNetwork();

    
    loadConfig();

    estimator.setup();
};

void System::systemUpdate()
{
    tunezhandler.update();
    sensors.update();
    estimator.update(sensors.getData());
    logTelemetry();
    // Serial.println(sensors.getData().accelgyro.ax);
};

void System::setupSPI()
{
    hspi.begin(PinMap::SNSR_SCLK,PinMap::SNSR_MISO,PinMap::SNSR_MOSI);
    hspi.setFrequency(8000000);
    hspi.setBitOrder(MSBFIRST);
    hspi.setDataMode(SPI_MODE0);
}


void System::setupPins()
{
    pinMode(PinMap::Accel_Cs, OUTPUT);
    pinMode(PinMap::Accel_Gyro_Cs, OUTPUT);
    pinMode(PinMap::Baro_Cs, OUTPUT);
    pinMode(PinMap::Mag_Cs, OUTPUT);
    pinMode(PinMap::IC_Cs, OUTPUT);
    // initialise cs pins
    digitalWrite(PinMap::Accel_Cs, HIGH);
    digitalWrite(PinMap::Accel_Gyro_Cs, HIGH);
    digitalWrite(PinMap::Baro_Cs, HIGH);
    digitalWrite(PinMap::Mag_Cs, HIGH);
    digitalWrite(PinMap::IC_Cs, HIGH);
}

void System::loadConfig()
{
    DynamicJsonDocument configDoc(16384); //allocate 16kb for config doc MAXSIZE
    DeserializationError jsonError;

    try
    {
        sensors.setup(configDoc.as<JsonObjectConst>()["Sensors"]);

    }
    catch (const std::exception &e)
    {
         RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Exception occured while loading flight config! - " + std::string(e.what()));

         throw e; //continue throwing as we dont want to continue
    }
   
}


void System::logTelemetry()
{
    if (micros() - prev_telemetry_log_time > telemetry_log_delta)
    {
        // RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(std::to_string(uxTaskGetStackHighWaterMark(primarysd.getHandle())));
        
        // std::string logstring = "int:" + std::to_string(usb_serial_jtag_ll_get_intsts_mask());
        // std::stringstream s;
        // s << std::hex << Serial.getRxQueue() <<"\n";
        // RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("sd card state: " + std::to_string(primarysd.getError()));

        const SensorStructs::raw_measurements_t& raw_sensors = sensors.getData();
        const SensorStructs::state_t& estimator_state =  estimator.getData();
        TelemetryLogframe logframe;
        
        
        logframe.ax = raw_sensors.accelgyro.ax;
        logframe.ay = raw_sensors.accelgyro.ay;
        logframe.az = raw_sensors.accelgyro.az;
        logframe.h_ax = raw_sensors.accel.ax;
        logframe.h_ay = raw_sensors.accel.ay;
        logframe.h_az = raw_sensors.accel.az;
        logframe.gx = raw_sensors.accelgyro.gx;
        logframe.gy = raw_sensors.accelgyro.gy;
        logframe.gz = raw_sensors.accelgyro.gz;
        logframe.mx = raw_sensors.mag.mx;
        logframe.my = raw_sensors.mag.my;
        logframe.mz = raw_sensors.mag.mz;
        logframe.imu_temp = raw_sensors.accelgyro.temp;
        logframe.baro_alt = raw_sensors.baro.alt;
        logframe.baro_temp = raw_sensors.baro.temp;
        logframe.baro_press = raw_sensors.baro.press;
        logframe.batt_volt = raw_sensors.logicrail.volt;
        logframe.batt_percent = raw_sensors.logicrail.percent;
        logframe.roll = estimator_state.eulerAngles[0];
        logframe.pitch = estimator_state.eulerAngles[1];
        logframe.yaw = estimator_state.eulerAngles[2];
        logframe.q0 = estimator_state.orientation.w();
        logframe.q1 = estimator_state.orientation.x();
        logframe.q2 = estimator_state.orientation.y();
        logframe.q3 = estimator_state.orientation.z();
        logframe.pn = estimator_state.position[0];
        logframe.pe = estimator_state.position[1];
        logframe.pd = estimator_state.position[2];
        logframe.vn = estimator_state.velocity[0];
        logframe.ve = estimator_state.velocity[1];
        logframe.vd = estimator_state.velocity[2];
        logframe.an = estimator_state.acceleration[0];
        logframe.ae = estimator_state.acceleration[1];
        logframe.ad = estimator_state.acceleration[2];

        logframe.timestamp = micros();

        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::TELEMETRY>(logframe);

        prev_telemetry_log_time = micros();
    }
}

void System::configureNetwork()
{   
    networkmanager.setNodeType(NODETYPE::HUB);

    networkmanager.enableAutoRouteGen(true);
    networkmanager.setNoRouteAction(NOROUTE_ACTION::BROADCAST, {1,3});

    RoutingTable flightRouting;
    flightRouting.setRoute((uint8_t)DEFAULT_ADDRESS::GROUNDSTATION_GATEWAY,Route{2,1,{}});
    flightRouting.setRoute((uint8_t)DEFAULT_ADDRESS::GROUNDSTATION,Route{2,2,{}});
    flightRouting.setRoute(17,Route{3,2,{}});
    flightRouting.setRoute(18,Route{3,2,{}});
    flightRouting.setRoute(5,Route{3,2,{}});
    flightRouting.setRoute(6,Route{3,2,{}});
    flightRouting.setRoute(7,Route{3,2,{}});
    flightRouting.setRoute(8,Route{3,2,{}});
    flightRouting.setRoute(9,Route{3,2,{}});
    flightRouting.setRoute(10,Route{3,2,{}});
    flightRouting.setRoute(11,Route{3,2,{}});
    flightRouting.setRoute(12,Route{3,2,{}});
    flightRouting.setRoute(13,Route{3,2,{}});
    flightRouting.setRoute(14,Route{3,2,{}});
    flightRouting.setRoute(15,Route{3,2,{}});
    flightRouting.setRoute(16,Route{3,2,{}});
    flightRouting.setRoute(50,Route{3,2,{}});
    flightRouting.setRoute(51,Route{3,2,{}});
    flightRouting.setRoute(52,Route{3,2,{}});
    flightRouting.setRoute(100,Route{3,2,{}});
    flightRouting.setRoute(101,Route{3,2,{}});
    flightRouting.setRoute(102,Route{3,2,{}});
    flightRouting.setRoute(150,Route{2,2,{}});

    
    networkmanager.setRoutingTable(flightRouting);
    networkmanager.updateBaseTable(); // save the new base table

};