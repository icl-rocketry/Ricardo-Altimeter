#include "record.h"

#include <memory>

#include <libriccore/fsm/state.h>
#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/commands/commandhandler.h>
#include <libriccore/riccorelogging.h>

#include "Config/systemflags_config.h"
#include "Config/types.h"

#include "system.h"


Record::Record(System& system) : State(SYSTEM_FLAG::STATE_RECORD,system.systemstatus),
_system(system)
{};

void Record::initialize()
{
    State::initialize(); // call parent initialize first!
    _system.commandhandler.enableCommands({Commands::ID::Free_Ram});
    digitalWrite(PinMap::LED_RED, HIGH);
    digitalWrite(PinMap::LED_BLUE, LOW);
    digitalWrite(PinMap::LED_GREEN, HIGH);
    counter = 0;
};

Types::CoreTypes::State_ptr_t Record::update()
{
    ESP_LOGI("Record", "In startup state");
    delay(200); //simulate doing startup tasks

    counter++;

    if(counter>=10){
        ESP_LOGI("Record", "Switching to MSC state");
        return std::make_unique<MSC>(_system);
    }
    return nullptr;
};

void Record::exit()
{
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};