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
_system(system),
logger()
{};

void Record::initialize()
{
    State::initialize(); // call parent initialize first!
    _system.commandhandler.enableCommands({Commands::ID::Free_Ram});
    digitalWrite(PinMap::LED_RED, HIGH);
    digitalWrite(PinMap::LED_BLUE, LOW);
    digitalWrite(PinMap::LED_GREEN, HIGH);
    logger.esplogData(_system);
    delay(1000); //wait a second to ensure logger output is seen before writing to FS
    // _system.filesystem.write_file("0:/test_0.txt", "Recording started\n", strlen("Recording started\n"), false); // worked successfully
    std::string line = logger.makeLogLine(_system);
    line += "\n";
    _system.filesystem.append_line("0:/test_0.txt", line.c_str());
};

Types::CoreTypes::State_ptr_t Record::update()
{
    // logger.esplogData(_system);
    delay(200); //simulate doing startup tasks

    return nullptr;
};

void Record::exit()
{
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};