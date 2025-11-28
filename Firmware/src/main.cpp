// main.cpp
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "tinyusb_cdc_acm.h"
#include "tinyusb_console.h"   // esp_tusb_init_console()
#include "tinyusb.h"
#include "tinyusb_default_config.h"

#include "system.h"
#include "LED/LED.h"
#include "Comms/Comms.h"

// static const char* TAG = "MAIN";

// // Tune these to your needs
// static constexpr uint32_t LOOP_STACK_BYTES = 4096;   // task stack size
// static constexpr UBaseType_t LOOP_PRIORITY  = 4;     // > idle/logging
// #if CONFIG_FREERTOS_UNICORE
// static constexpr BaseType_t LOOP_CORE = tskNO_AFFINITY;
// #else
// static constexpr BaseType_t LOOP_CORE = 1;           // S3: core 1 is common for app tasks
// #endif

static System gSystem;

static void system_task(void* /*arg*/)
{
    ESP_LOGI(TAG, "system_task start");
    gSystem.systemSetup();                 // run once

    for (;;) {                             // run forever
        gSystem.systemUpdate();
        vTaskDelay(pdMS_TO_TICKS(1));      // yield (adjust rate as needed)
    }
}

void usbTask(void *arg) {
    while (1) {
        tud_task();  // tinyusb internal processing
        vTaskDelay(1);
    }
}

void tinyusbInitTask(void *arg) {
    vTaskDelay(pdMS_TO_TICKS(200));  // allow system to stabilize
    CDCHandler *cdc = (CDCHandler*)arg;
    cdc->setup();  // we create a new init() function
    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    RGBLed aliveLed;    
    aliveLed.blue();
    
    CDCHandler cdcHandler([&aliveLed](){
        aliveLed.red();
    });
    
    xTaskCreate(usbTask, "usbTask", 4096, NULL, 5, NULL);
    
    cdcHandler.setup();
    aliveLed.green();
    // // then initialize TinyUSB in a deferred task
    // xTaskCreate(tinyusbInitTask, "tinyusbInitTask", 4096, &cdcHandler, 5, NULL);

    ESP_LOGI(TAG, "app_main");
    TaskHandle_t handle = nullptr;
    BaseType_t ok = xTaskCreatePinnedToCore(
        system_task,
        "system_task",
        LOOP_STACK_BYTES,
        nullptr,
        LOOP_PRIORITY,
        &handle,
        LOOP_CORE
    );
    if (ok != pdPASS) {
        ESP_LOGE(TAG, "Failed to create system_task");
    }
}
