#include "startup.h"

#include <memory>

#include <libriccore/fsm/state.h>
#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/commands/commandhandler.h>
#include <libriccore/riccorelogging.h>

#include "Config/systemflags_config.h"
#include "Config/types.h"

#include "system.h"


Startup::Startup(Types::CoreTypes::SystemStatus_t& systemtatus, Types::CoreTypes::CommandHandler_t& commandhandler):
State(SYSTEM_FLAG::STATE_STARTUP,systemtatus),
_commandhandler(commandhandler)
{};

void Startup::initialize()
{
    State::initialize(); // call parent initialize first!
    _commandhandler.enableCommands({Commands::ID::Free_Ram});
    digitalWrite(PinMap::LED_RED, HIGH);
    digitalWrite(PinMap::LED_BLUE, HIGH);
    digitalWrite(PinMap::LED_GREEN, LOW);
    counter = 0;
};

Types::CoreTypes::State_ptr_t Startup::update()
{
    ESP_LOGI("Startup", "In startup state");
    delay(200); //simulate doing startup tasks

    counter++;

    if(counter>=10){
        ESP_LOGI("Startup", "Switching to MSC state");
        return std::make_unique<MSC>(this->_systemstatus,this->_commandhandler);
    }
    return nullptr;
};

void Startup::exit()
{
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};