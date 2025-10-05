#include "msc.h"

#include <memory>

#include <libriccore/fsm/state.h>
#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/commands/commandhandler.h>
#include <libriccore/riccorelogging.h>

#include "Config/systemflags_config.h"
#include "Config/types.h"

#include "system.h"


MSC::MSC(Types::CoreTypes::SystemStatus_t& systemtatus, Types::CoreTypes::CommandHandler_t& commandhandler):
State(SYSTEM_FLAG::STATE_MSC,systemtatus),
_commandhandler(commandhandler)
{};

void MSC::initialize()
{
    State::initialize(); // call parent initialize first!
    _commandhandler.enableCommands({Commands::ID::Free_Ram});

    digitalWrite(PinMap::LED_RED, LOW);
    digitalWrite(PinMap::LED_BLUE, HIGH);
    digitalWrite(PinMap::LED_GREEN, HIGH);

    // 1) Start TinyUSB
    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    // 2) Bring up CDC-ACM (note: no .usb_dev field in this API)
    tinyusb_config_cdcacm_t cdc_cfg = {
        .cdc_port = TINYUSB_CDC_ACM_0,
        .callback_rx = NULL,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL,
    };
    ESP_ERROR_CHECK(tinyusb_cdcacm_init(&cdc_cfg));

    // 3) Route the console/logs to CDC so you keep logs while MSC is active
    ESP_ERROR_CHECK(tinyusb_console_init(TINYUSB_CDC_ACM_0));

    // 4) Your MSC init as before
    tinyusb_msc_driver_config_t msc_cfg = { .callback = msc_evt_cb };
    ESP_ERROR_CHECK(tinyusb_msc_install_driver(&msc_cfg));
    
};

Types::CoreTypes::State_ptr_t MSC::update()
{
    ESP_LOGI("MSC", "In MSC state");
    delay(200); //simulate doing startup tasks

    return nullptr;
};

void MSC::exit()
{
    Types::CoreTypes::State_t::exit(); // call parent exit last!
};


