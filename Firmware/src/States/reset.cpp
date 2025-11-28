// #include "reset.h"

// #include <memory>

// #include <libriccore/fsm/state.h>
// #include <libriccore/systemstatus/systemstatus.h>
// #include <libriccore/commands/commandhandler.h>
// #include <libriccore/riccorelogging.h>

// #include "Config/systemflags_config.h"
// #include "Config/types.h"

// #include "system.h"


// Reset::Reset(System& system) : State(SYSTEM_FLAG::STATE_RESET,system.systemstatus),
// _system(system)
// {};

// void Reset::initialize()
// {
//     State::initialize(); // call parent initialize first!
//     _system.commandhandler.enableCommands({Commands::ID::Free_Ram});
//     digitalWrite(PinMap::LED_RED, HIGH);
//     digitalWrite(PinMap::LED_BLUE, LOW);
//     digitalWrite(PinMap::LED_GREEN, LOW);
// };

// Types::CoreTypes::State_ptr_t Reset::update()
// {
//     _system.filesystem.clear_all_files();

//     return std::make_unique<MSC>(_system);
// };

// void Reset::exit()
// {
//     Types::CoreTypes::State_t::exit(); // call parent exit last!
// };