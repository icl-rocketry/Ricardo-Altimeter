#include "log.h"

#include <memory>

#include <libriccore/fsm/state.h>
#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/commands/commandhandler.h>
#include <libriccore/riccorelogging.h>

#include "Config/systemflags_config.h"
#include "Config/types.h"

#include "system.h"


Log::Log(System& system) : State(SYSTEM_FLAG::STATE_LOG,system.systemstatus),
_system(system),
logger()
{};

void Log::initialize()
{
    State::initialize(); // call parent initialize first!
    _system.commandhandler.enableCommands({Commands::ID::Free_Ram});
    digitalWrite(PinMap::LED_RED, LOW);
    digitalWrite(PinMap::LED_BLUE, LOW);
    digitalWrite(PinMap::LED_GREEN, HIGH);
};

Types::CoreTypes::State_ptr_t Log::update()
{
    if (millis() - last_log_time > 50) {
        std::string line = logger.makePrintLine(_system);
        ESP_LOGI("Log State", "%s", line.c_str());
        last_log_time = millis();
    }

    return nullptr;
};

void Log::exit()
{
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};