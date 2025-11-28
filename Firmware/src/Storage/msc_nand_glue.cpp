// extern "C" {
//   #include "class/msc/msc.h"           // SCSI_SENSE_* macros
//   #include "class/msc/msc_device.h"    // tud_msc_* prototypes, tud_msc_set_sense()
//   #include "tusb.h"
// }
// #include "esp_log.h"
// #include "file_system.hpp"

// static const char *TAG = "MSC-NAND";
// static FileSystem* g_fs = nullptr;
// static bool g_ejected = false;

// // Call this BEFORE TinyUSB enumeration, once your FS object exists.
// extern "C" void msc_nand_attach(FileSystem* fs) {
//     g_fs = fs;
//     // Ensure the app is not mounted while host will own the medium
//     g_fs->handover_to_usb();

//     uint32_t nsec = g_fs->sector_count_512();
//     ESP_LOGI(TAG, "Attach LUN0: %lu sectors x 512B = %lu KiB",
//          (unsigned long)nsec,
//          (unsigned long)(((uint64_t)nsec * 512) / 1024));
// }

// extern "C" bool tud_msc_test_unit_ready_cb(uint8_t lun) {
//     if (!g_fs) return false;                 // ok to tell host we're not ready until attach
//     if (g_ejected) {                         // only after a real eject
//         tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
//         return false;
//     }
//     if (g_fs->sector_count_512() == 0) {
//         tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
//         return false;
//     }
//     return true;   // IMPORTANT: say “ready” so host will mount it
// }

// extern "C" void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size) {
//     *block_size  = 512;
//     *block_count = g_fs ? g_fs->sector_count_512() : 0;
// }

// extern "C" bool tud_msc_is_writable_cb(uint8_t /*lun*/) { return true; }

// static uint8_t s_bounce[512];

// static inline bool lba_ok(uint32_t lba, uint32_t blocks) {
//     if (!g_fs) return false;
//     uint32_t cap = g_fs->sector_count_512();
//     return (lba < cap) && (blocks <= cap - lba);
// }

// extern "C" int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset,
//                                      void *buffer, uint32_t bufsize) {
//     if (!g_fs) return -1;

//     uint32_t first = lba + offset / 512;
//     uint32_t last  = lba + (offset + bufsize - 1) / 512;
//     uint32_t nblk  = last - first + 1;
//     if (!lba_ok(first, nblk)) {
//         tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x21, 0x00); // LBA out of range
//         return -1;
//     }

//     uint32_t done = 0;
//     while (done < bufsize) {
//         uint32_t lba_i = lba + (offset + done) / 512;
//         uint32_t off_i = (offset + done) % 512;
//         uint32_t chunk = 512 - off_i;
//         if (chunk > bufsize - done) chunk = bufsize - done;

//         if (off_i == 0 && chunk == 512) {
//             if (g_fs->disk_read((uint8_t*)buffer + done, lba_i, 1) != RES_OK) return -1;
//         } else {
//             if (g_fs->disk_read(s_bounce, lba_i, 1) != RES_OK) return -1;
//             memcpy((uint8_t*)buffer + done, s_bounce + off_i, chunk);
//         }
//         done += chunk;
//     }
//     return (int32_t)bufsize;
// }

// extern "C" int32_t tud_msc_write10_cb (uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
//     if (!g_fs) return -1;

//     uint32_t first = lba + offset / 512;
//     uint32_t last  = lba + (offset + bufsize - 1) / 512;
//     uint32_t nblk  = last - first + 1;
//     if (!lba_ok(first, nblk)) {
//         tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x21, 0x00); // LBA out of range
//         return -1;
//     }

//     uint32_t done = 0;
//     while (done < bufsize) {
//         uint32_t lba_i = lba + (offset + done) / 512;
//         uint32_t off_i = (offset + done) % 512;
//         uint32_t chunk = 512 - off_i;
//         if (chunk > bufsize - done) chunk = bufsize - done;

//         if (off_i == 0 && chunk == 512) {
//             if (g_fs->disk_write((uint8_t*)buffer + done, lba_i, 1) != RES_OK) return -1;
//         } else {
//             // R-M-W for partial sector
//             if (g_fs->disk_read(s_bounce, lba_i, 1) != RES_OK) return -1;
//             memcpy(s_bounce + off_i, (uint8_t const*)buffer + done, chunk);
//             if (g_fs->disk_write(s_bounce, lba_i, 1) != RES_OK) return -1;
//         }
//         done += chunk;
//     }
//     return (int32_t)bufsize;
// }

// extern "C" bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
//     // Eject (host requests removal)
//     if (load_eject && !start) {
//         g_ejected = true;
//         if (g_fs) (void)g_fs->disk_ioctl(CTRL_SYNC, nullptr); // flush FTL/FAT metadata
//         ESP_LOGI(TAG, "Host ejected LUN0");
//     }
//     // Load (host reinserts)
//     if (load_eject && start) {
//         g_ejected = false;
//         ESP_LOGI(TAG, "Host loaded LUN0");
//     }
//     return true;
// }

// // Let TinyUSB handle unknown SCSI
// extern "C" int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) { return false; }

// extern "C" void tud_msc_inquiry_cb(uint8_t /*lun*/,
//                                    uint8_t vendor_id[8],
//                                    uint8_t product_id[16],
//                                    uint8_t product_rev[4]) {
//     // Must be space-padded, not null-terminated
//     memset(vendor_id,  ' ', 8);
//     memset(product_id, ' ', 16);
//     memset(product_rev,' ', 4);

//     // Fill with your strings (max 8/16/4 chars)
//     memcpy(vendor_id,   "RICARDO", 7);       // 7 chars -> 1 trailing space remains
//     memcpy(product_id,  "NAND STORAGE", 12); // leaves spaces to 16
//     memcpy(product_rev, "1.0", 3);           // leaves one space
// }


