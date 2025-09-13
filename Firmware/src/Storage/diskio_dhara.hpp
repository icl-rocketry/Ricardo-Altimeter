#pragma once
#include <stdint.h>
#include "file_system.hpp"

extern "C" {
  #include "diskio.h"       // FatFs types (BYTE, DWORD, etc.)
  #include "diskio_impl.h"  // ff_diskio_register + ff_diskio_impl_t
}

void file_system_register_diskio(uint8_t pdrv);
void file_system_unregister_diskio(uint8_t pdrv);
