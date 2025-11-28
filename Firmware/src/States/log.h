// #pragma once

// #include <memory>

// #include <libriccore/fsm/state.h>
// #include <libriccore/systemstatus/systemstatus.h>
// #include <libriccore/commands/commandhandler.h>

// #include "Config/pinmap_config.h"
// #include "Config/systemflags_config.h"
// #include "Config/types.h"
// #include "Sensors/logger.h"

// class Log : public Types::CoreTypes::State_t
// {
//     public:
//         Log(System& system);

//         void initialize() override;

//         Types::CoreTypes::State_ptr_t update() override;

//         void exit() override;

//     private:
//         System& _system;
//         Logger logger;
//         std::string file_name;
//         int counter;
//         uint32_t last_log_time;
// };