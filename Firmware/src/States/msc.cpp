// #include "msc.h"

// #include <memory>

// #include <libriccore/fsm/state.h>
// #include <libriccore/systemstatus/systemstatus.h>
// #include <libriccore/commands/commandhandler.h>
// #include <libriccore/riccorelogging.h>

// #include "Config/systemflags_config.h"
// #include "Config/types.h"

// #include "system.h"


// MSC::MSC(Types::CoreTypes::SystemStatus_t& systemtatus, Types::CoreTypes::CommandHandler_t& commandhandler):
// State(SYSTEM_FLAG::STATE_MSC,systemtatus),
// _commandhandler(commandhandler)
// {};

// void MSC::initialize()
// {
//     State::initialize(); // call parent initialize first!
//     _commandhandler.enableCommands({Commands::ID::Free_Ram});

//     digitalWrite(PinMap::LED_RED, LOW);
//     digitalWrite(PinMap::LED_BLUE, HIGH);
//     digitalWrite(PinMap::LED_GREEN, HIGH);

//     ESP_LOGI("MSC", "hello from ESP-IDF logs");
//     delay(200);

//     // // 1) Start TinyUSB
//     // tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
//     // ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

//     // // 2) Install the MSC driver (lets you get events & control auto-mount behavior)
//     // tinyusb_msc_driver_config_t dcfg = {0};
//     // // dcfg.user_flags.auto_mount_off = 1; // set if you want to manage mount switching yourself
//     // dcfg.callback     = msc_evt_cb;
//     // dcfg.callback_arg = NULL;
//     // ESP_ERROR_CHECK(tinyusb_msc_install_driver(&dcfg));
    
// };

// Types::CoreTypes::State_ptr_t MSC::update()
// {
//     return nullptr;
// };

// void MSC::exit()
// {
//     Types::CoreTypes::State_t::exit(); // call parent exit last!
// };


