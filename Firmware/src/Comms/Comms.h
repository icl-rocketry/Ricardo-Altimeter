#pragma once

#include <functional>

// #include "esp_usb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "tinyusb_cdc_acm.h"
#include "tinyusb_console.h"   // esp_tusb_init_console()
#include "tinyusb.h"
#include "tinyusb_default_config.h"

#include "Comms/CDC.h"
static const char* TAG = "MAIN";

// Tune these to your needs
static constexpr uint32_t LOOP_STACK_BYTES = 4096;   // task stack size
static constexpr UBaseType_t LOOP_PRIORITY  = 4;     // > idle/logging
#if CONFIG_FREERTOS_UNICORE
static constexpr BaseType_t LOOP_CORE = tskNO_AFFINITY;
#else
static constexpr BaseType_t LOOP_CORE = 1;           // S3: core 1 is common for app tasks
#endif



const char *string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: English (US)
    "My Company",               // 1: Manufacturer
    "My USB Device",            // 2: Product
    "12345678",                 // 3: Serial
};

class CDCHandler {
public:
    CDCHandler(std::function<void()> onError)
    : onError(onError)
    {}

    void setup() {
        tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();

        if (tinyusb_driver_install(&tusb_cfg) != ESP_OK) {
            onError();
        }

        tinyusb_config_cdcacm_t cdc_cfg = {
            .cdc_port = TINYUSB_CDC_ACM_0,
            .callback_rx = my_tud_cdc_rx_cb,
        };

        if (tinyusb_cdcacm_init(&cdc_cfg) != ESP_OK) {
            onError();
        }

        tinyusb_console_init(TINYUSB_CDC_ACM_0);
    }

private:
    std::function<void()> onError;

};

