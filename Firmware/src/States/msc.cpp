// #include "msc.h"

// #include <memory>

// #include <libriccore/fsm/state.h>
// #include <libriccore/systemstatus/systemstatus.h>
// #include <libriccore/commands/commandhandler.h>
// #include <libriccore/riccorelogging.h>

// #include "Config/systemflags_config.h"
// #include "Config/types.h"

// #include "system.h"


// MSC::MSC(System& system) : State(SYSTEM_FLAG::STATE_MSC,system.systemstatus),
// _system(system)
// {};

// // -------- Interface numbers (CDC has 2: control + data)
// enum {
//   ITF_NUM_CDC = 0,
//   ITF_NUM_CDC_DATA,
//   ITF_NUM_MSC,
//   ITF_NUM_TOTAL
// };

// // -------- Endpoint addresses (FS, unique per direction)
// #define EPNUM_CDC_NOTIF  0x81
// #define EPNUM_CDC_OUT    0x02
// #define EPNUM_CDC_IN     0x82
// #define EPNUM_MSC_OUT    0x03
// #define EPNUM_MSC_IN     0x83

// // total length: config + CDC + MSC
// #define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_MSC_DESC_LEN)

// // Full-speed composite: CDC + MSC
// static const uint8_t fs_cfg_desc[] = {
//   // bConfigurationValue=1, interface count, iConfiguration=0, total len, attributes, power mA
//   TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
//                         TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

//   // CDC (ACM) — 2 interfaces, 3 endpoints
//   // itf = ITF_NUM_CDC, iFuncStr=0, notif EP, notif size, OUT EP, IN EP, EP packet size
//   TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 0, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),

//   // MSC — 1 interface, 2 endpoints
//   // itf = ITF_NUM_MSC, iFuncStr=0, OUT EP, IN EP, EP packet size
//   TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
// };

// // Language + strings (keep indexes consistent with your device descriptor)
// static const char *strings[] = {
//   (const char[]){ 0x09, 0x04 },   // 0: English (US)
//   "Ricardo",                      // 1: Manufacturer
//   "MSC+CDC Device",               // 2: Product
//   "123456",                       // 3: Serial
// };

// void MSC::initialize()
// {
//     State::initialize(); // call parent initialize first!
//     _system.commandhandler.enableCommands({Commands::ID::Free_Ram});

//     digitalWrite(PinMap::LED_RED, LOW);
//     digitalWrite(PinMap::LED_BLUE, HIGH);
//     digitalWrite(PinMap::LED_GREEN, HIGH);


//     msc_nand_attach(&_system.filesystem);
    
//     tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
//     tusb_cfg.descriptor.full_speed_config = fs_cfg_desc;
//     tusb_cfg.descriptor.string            = strings;
//     tusb_cfg.descriptor.string_count      = sizeof(strings)/sizeof(strings[0]);

//     ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

//     // 2) Bring up CDC-ACM (note: no .usb_dev field in this API)
//     tinyusb_config_cdcacm_t cdc_cfg = {
//         .cdc_port = TINYUSB_CDC_ACM_0,
//         .callback_rx = NULL,
//         .callback_rx_wanted_char = NULL,
//         .callback_line_state_changed = NULL,
//         .callback_line_coding_changed = NULL,
//     };
//     ESP_ERROR_CHECK(tinyusb_cdcacm_init(&cdc_cfg));
//     ESP_ERROR_CHECK(tinyusb_console_init(TINYUSB_CDC_ACM_0));

// };

// Types::CoreTypes::State_ptr_t MSC::update()
// {
//     ESP_LOGI("MSC", "In MSC state");
//     delay(200); //simulate doing startup tasks
//     digitalWrite(PinMap::LED_RED, !digitalRead(PinMap::LED_RED));

//     return nullptr;
// };

// void MSC::exit()
// {
//     Types::CoreTypes::State_t::exit(); // call parent exit last!
// };




// // for some reason logs no longer work

// // (engineering) jack@jak:~/Documents/iclr/Ricardo-Altimeter/Firmware$ ls -l /dev/serial/by-id/
// // total 0
// // lrwxrwxrwx 1 root root 13 Oct  5 21:51 usb-Espressif_Systems_Espressif_Device_123456-if00 -> ../../ttyACM0 ~~~~~~~~~ correct
// // (engineering) jack@jak:~/Documents/iclr/Ricardo-Altimeter/Firmware$ ls -l /dev/serial/by-id/
// // total 0
// // lrwxrwxrwx 1 root root 13 Oct  5 21:57 usb-Espressif_USB_JTAG_serial_debug_unit_F4:12:FA:9A:D3:C4-if00 -> ../../ttyACM0 ~~~~~~~~~ current