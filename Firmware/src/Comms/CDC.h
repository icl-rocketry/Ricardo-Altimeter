#include "tusb.h"
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "tinyusb_cdc_acm.h"

#define CMD_BUFFER_SIZE 128

static char cmdBuffer[CMD_BUFFER_SIZE];
static size_t cmdIndex = 0;

void processCommand(const char *cmd) {
        if (strcmp(cmd, "FLASH") == 0) {
            ESP_LOGW("CMD", "Rebooting into USB bootloader...");
            vTaskDelay(pdMS_TO_TICKS(50));  // allow USB to flush
        }

        // Add other commands here:
        // if (strcmp(cmd, "RED") == 0) { aliveLed.red(); }
    }

extern "C" void tud_cdc_rx_cb(uint8_t itf)
{
    // while (tud_cdc_available())
    // {
    //     char c = tud_cdc_read_char();

    //     // newline ends a command
    //     if (c == '\n' || c == '\r') {
    //         if (cmdIndex > 0) {
    //             cmdBuffer[cmdIndex] = '\0';
    //             processCommand(cmdBuffer);
    //             cmdIndex = 0;
    //         }
    //     } 
    //     else {
    //         if (cmdIndex < CMD_BUFFER_SIZE - 1) {
    //             cmdBuffer[cmdIndex++] = c;
    //         }
    //     }
    // }
    ESP_LOGI("CDC", "RX callback fired!");

    while (tud_cdc_available())
    {
        int c = tud_cdc_read_char();
        ESP_LOGI("CDC", "Got char: '%c' (0x%02X)", c, c);

        if (c == '\n' || c == '\r') {
            cmdBuffer[cmdIndex] = '\0';
            ESP_LOGI("CDC", "Command: %s", cmdBuffer);
            processCommand(cmdBuffer);
            cmdIndex = 0;
        } else {
            cmdBuffer[cmdIndex++] = c;
        }
    }
}

extern "C" void my_tud_cdc_rx_cb(int itf, cdcacm_event_t *event)
{
    ESP_LOGI("CDC", "RX callback fired!");

    while (tud_cdc_available())
    {
        int c = tud_cdc_read_char();
        ESP_LOGI("CDC", "Got char: '%c' (0x%02X)", c, c);

        if (c == '\n' || c == '\r') {
            cmdBuffer[cmdIndex] = '\0';
            ESP_LOGI("CDC", "Command: %s", cmdBuffer);
            processCommand(cmdBuffer);
            cmdIndex = 0;
        } else {
            cmdBuffer[cmdIndex++] = c;
        }
    }
}


#include "driver/gpio.h"
#include "esp_rom_sys.h"

void enter_bootloader_gpio()
{
    // GPIO0 = BOOT
    gpio_reset_pin(GPIO_NUM_0);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_0, 0);     // BOOT mode

    // RESET = EN / CHIP_PU
    gpio_reset_pin(GPIO_NUM_3);        // adjust if your reset pin differs
    gpio_set_direction(GPIO_NUM_3, GPIO_MODE_OUTPUT);

    // Pulse reset
    gpio_set_level(GPIO_NUM_3, 0);
    esp_rom_delay_us(50000);          // 50ms reset
    gpio_set_level(GPIO_NUM_3, 1);

    // Never returns; chip reboots
}