#pragma once

#include <SPI.h>


#include <libriccore/riccoresystem.h>


#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"

#include "Commands/commands.h"

#include "States/idle.h"

#include "Storage/nand_flash.hpp"
#include "Storage/file_system.hpp"

class System : public RicCoreSystem<System,SYSTEM_FLAG,Commands::ID>
{
    public:

        System();
        
        void systemSetup();

        void systemUpdate();

    private:

        void setupSPI();
        void setupPins();

        SPIClass vspi;
        SPIClass hspi;

        NANDFlash nandflash;
        FileSystem filesystem;

};