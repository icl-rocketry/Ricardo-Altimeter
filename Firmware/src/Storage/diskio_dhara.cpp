#include "diskio_dhara.hpp"

// Callbacks that forward to your FileSystem instance
static DSTATUS dhara_init   (BYTE pdrv) { return FileSystem::instance()->disk_initialize(); }
static DSTATUS dhara_status (BYTE pdrv) { return 0; }

static DRESULT dhara_read   (BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
  return FileSystem::instance()->disk_read(buff, (uint32_t)sector, (uint32_t)count);
}
static DRESULT dhara_write  (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
  return FileSystem::instance()->disk_write(buff, (uint32_t)sector, (uint32_t)count);
}
static DRESULT dhara_ioctl  (BYTE pdrv, BYTE cmd, void* buff) {
  return FileSystem::instance()->disk_ioctl(cmd, buff);
}

// Public helpers (you can also make these FileSystem methods)
void file_system_register_diskio(uint8_t pdrv) {
  static const ff_diskio_impl_t impl = {
    .init   = dhara_init,
    .status = dhara_status,
    .read   = dhara_read,
    .write  = dhara_write,
    .ioctl  = dhara_ioctl,
  };
  ff_diskio_register(pdrv, &impl);
}

void file_system_unregister_diskio(uint8_t pdrv) {
  // Unregister (ESP-IDF uses NULL to unregister this pdrv)
  ff_diskio_register(pdrv, NULL);
}
