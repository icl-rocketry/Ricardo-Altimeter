#pragma once

#include <SPI.h>


#include <libriccore/riccoresystem.h>


#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"

#include "Commands/commands.h"

#include "Sensors/sensors.h"
#include "Sensors/estimator.h"

#include "States/startup.h"

#include "Storage/nand_flash.hpp"
#include "Storage/file_system.hpp"

#include "esp_log.h"

class System : public RicCoreSystem<System,SYSTEM_FLAG,Commands::ID>
{
    public:

        System();
        
        void systemSetup();

        void systemUpdate();

        SPIClass vspi;
        SPIClass hspi;
        
        Sensors sensors;
        Estimator estimator;

    private:

        void setupSPI();
        void setupPins();
        void loadConfig();


        NANDFlash nandflash;
        FileSystem filesystem;

};