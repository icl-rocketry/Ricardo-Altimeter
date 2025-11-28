// #pragma once

// #include <memory>

// #include <libriccore/fsm/state.h>
// #include <libriccore/systemstatus/systemstatus.h>
// #include <libriccore/commands/commandhandler.h>

// #include "Config/pinmap_config.h"
// #include "Config/systemflags_config.h"
// #include "Config/types.h"
// #include "States/msc.h"
// #include "States/record.h"
// #include "States/reset.h"
// #include "States/log.h"

// class Startup : public Types::CoreTypes::State_t
// {
//     public:
//         Startup(System& system);

//         void initialize() override;

//         Types::CoreTypes::State_ptr_t update() override;

//         void exit() override;

//     private:
//         System& _system;
//         uint32_t counter;
// };