#pragma once

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "Config/systemflags_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"

#include "Sensors/sensors.h"
#include "Sensors/estimator.h"

#include "States/startup.h"

#include "Storage/nand_flash.hpp"
#include "Storage/file_system.hpp"
#include "Commands/packets/telemetry_packet.h"

class System
{
    public:

        System();
        
        void systemSetup();

        void systemUpdate();

        
        Sensors sensors;
        TelemetryPacket telemetry;
        // Estimator estimator;

        // NANDFlash nandflash;
        // FileSystem filesystem;
    private:

        void setupSPI();
        void setupPins();

        static constexpr char TAG[] = "SYSTEM";

};