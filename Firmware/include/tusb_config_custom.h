// tusb_config_custom.h
#pragma once
#include "sdkconfig.h"

#define CFG_TUSB_MCU           OPT_MCU_ESP32S3
#define CFG_TUSB_OS            OPT_OS_FREERTOS
#define CFG_TUSB_RHPORT0_MODE  (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#define CFG_TUD_ENDPOINT0_SIZE 64

// Classes you actually use
#define CFG_TUD_CDC            1
#define CFG_TUD_MSC            1

// Buffers
#define CFG_TUD_MSC_EP_BUFSIZE 512
#define CFG_TUD_CDC_EP_BUFSIZE 64
#define CFG_TUD_CDC_RX_BUFSIZE 256
#define CFG_TUD_CDC_TX_BUFSIZE 256
