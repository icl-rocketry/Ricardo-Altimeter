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

    delay(1000); //wait a second to ensure logger output is seen before writing to FS

    int current_number_of_files = _system.filesystem.getNumberOfFiles();
    file_name = "0:/LOG";
    if (current_number_of_files < 10) {
        file_name += "0" + std::to_string(current_number_of_files) + ".txt";
    } else {
        file_name += std::to_string(current_number_of_files) + ".txt";
    }

    auto header = logger.makeHeaderLine();

    _system.filesystem.write_file(file_name.c_str(), header.c_str(), header.size(), false);
    last_log_time = millis();
};

Types::CoreTypes::State_ptr_t Record::update()
{
    if (millis() - last_log_time > 50) {
        digitalWrite(PinMap::LED_RED, !digitalRead(PinMap::LED_RED));
        std::string line = logger.makeLogLine(_system);
        _system.filesystem.append_line(file_name.c_str(), line.c_str());
        last_log_time = millis();
    }

    return nullptr;
};

void Record::exit()
{
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};