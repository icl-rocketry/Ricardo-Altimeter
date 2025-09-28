#include "file_system.hpp"

static constexpr uint8_t kDrv = 0;

static FileSystem *g_fs = nullptr;
FileSystem *FileSystem::instance() { return g_fs; }

FileSystem::FileSystem(NANDFlash &nandflash) : nandflash_(nandflash)
{
    g_fs = this;
}

bool FileSystem::setup(bool mkfs_if_needed)
{
    // 1) Bring up raw NAND
    auto result = nandflash_.setup();
    if (result != NANDFlash::Error::SUCCESS)
    {
        ESP_LOGI(TAG, "NAND Flash Setup Failed %d", (int)result);
        return false;
    }
    ESP_LOGI(TAG, "NAND Flash Setup Success");

    tf_.init(nandflash_, /*log2_page_size*/ 11, /*log2_ppb*/ 6, /*num_blocks*/ 4096);
    dhara_map_init(&map_, &tf_.dn, dhara_page_buf_, /*gc_ratio=*/1);

    dhara_error_t e = DHARA_E_NONE;
    if (dhara_map_resume(&map_, &e) == 0)
    {
        ESP_LOGI(TAG, "Dhara resume OK");
    }
    else
    {
        ESP_LOGI(TAG, "Dhara resume failed (%d).\n", (int)e);
    }

    // Register our disk I/O driver BEFORE f_mount
    file_system_register_diskio(kDrv);

    // Mount using the same drive number
    FRESULT fr = f_mount(&fs_, "0:", 1);
    ESP_LOGI(TAG, "f_mount returned %d", (int)fr);
    if (fr == FR_NO_FILESYSTEM && mkfs_if_needed)
    {
        ESP_LOGI(TAG, "No filesystem found — creating FAT...");
        if (!format(/*au_kb=*/16))
            return false;
        fr = f_mount(&fs_, "0:", 1);
    }
    if (fr != FR_OK)
    {
        ESP_LOGI(TAG, "f_mount failed (%d)", (int)fr);
        file_system_unregister_diskio(kDrv);
        return false;
    }
    mounted_ = true;
    return true;
}

void FileSystem::format_flash() {
    auto result = nandflash_.setup();
    if (result != NANDFlash::Error::SUCCESS) {
        ESP_LOGI(TAG, "NAND Flash Setup Failed %u", static_cast<uint8_t>(result));
    } else {
        ESP_LOGI(TAG, "NAND Flash Setup Success");
    }

    tf_.init(nandflash_, /*log2_page_size*/11, /*log2_ppb*/6, /*num_blocks*/4096);

    dhara_map_init(&map_, &tf_.dn, dhara_page_buf_, /*gc_ratio=*/1);

    const size_t PSZ = 1u << tf_.dn.log2_page_size;   // 2048
    static uint8_t page[1 << 11];                     // 2 KiB
    memset(page, 0, PSZ);

    dhara_error_t e = DHARA_E_NONE;

    if (dhara_map_resume(&map_, &e) == 0) {
        ESP_LOGI(TAG, "Dhara resume OK");
    } else {
        ESP_LOGI(TAG, "Resume failed (%d). Formatting via write+sync…", (int)e);

        if (dhara_map_write(&map_, /*lba*/0, page, &e) < 0) {
            ESP_LOGI(TAG, "format write failed: %d", (int)e);
        } else if (dhara_map_sync(&map_, &e) < 0) {
            ESP_LOGI(TAG, "format sync failed: %d", (int)e);
        } else {
            if (dhara_map_resume(&map_, &e) == 0)
                ESP_LOGI(TAG, "Resume OK after write+sync");
            else
                ESP_LOGI(TAG, "Resume still failed: %d", (int)e);
        }
    }
}

void FileSystem::print_disk_space() {
    DWORD fre_clust = 0;
    FATFS* ignore = nullptr;               // required by API but we won't use it

    FRESULT fr = f_getfree("0:", &fre_clust, &ignore);
    ESP_LOGI(TAG, "f_getfree -> FR=%d", (int)fr);
    if (fr != FR_OK) return;

    const uint64_t cluster_bytes = (uint64_t)fs_.csize * 512ULL;
    const uint64_t total  = (uint64_t)(fs_.n_fatent - 2) * cluster_bytes;
    const uint64_t free_b = (uint64_t)fre_clust          * cluster_bytes;

    ESP_LOGI(TAG, "FAT total=%llu bytes, free=%llu bytes",
                    (unsigned long long)total,
                    (unsigned long long)free_b);
}

void FileSystem::print_files(const char *start)
{
    if (!mounted_) {
        ESP_LOGI(TAG, "Filesystem not mounted");
        return;
    }

    (void)disk_ioctl(CTRL_SYNC, nullptr);  // flush pending writes to NAND/Dhara

    const char *root = (start && *start) ? start : "0:/";
    uint32_t files = 0, dirs = 0;
    uint64_t bytes = 0;

    ESP_LOGI(TAG, "Listing files under '%s'", root);
    list_dir_recursive(root, 0, files, dirs, bytes);

    ESP_LOGI(TAG, "Summary: %lu files, %lu dirs, %llu bytes total",
                  (unsigned long)files, (unsigned long)dirs,
                  (unsigned long long)bytes);
}

void FileSystem::list_dir_recursive(const char *path, int depth,
                                    uint32_t &files, uint32_t &dirs, uint64_t &bytes)
{
    FF_DIR dir;
    FILINFO fno;

    FRESULT fr = f_opendir(&dir, path);
    if (fr != FR_OK) {
        ESP_LOGI(TAG, "f_opendir('%s') -> %d", path, (int)fr);
        return;
    }

    char indent[128];

    for (;;) {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK) {
            ESP_LOGI(TAG, "f_readdir('%s') -> %d", path, (int)fr);
            break;
        }
        if (fno.fname[0] == '\0') {
            // End of directory
            break;
        }

        const char *name = fno.fname;
        make_indent(depth, indent, sizeof(indent));

        if (fno.fattrib & AM_DIR) {
            dirs++;
            ESP_LOGI(TAG, "%s[DIR] %s", indent, name);

            char next[256];
            bool has_slash = (path[0] && path[strlen(path) - 1] == '/');
            int n = snprintf(next, sizeof(next), "%s%s%s", path, has_slash ? "" : "/", name);
            if (n > 0 && (size_t)n < sizeof(next)) {
                list_dir_recursive(next, depth + 1, files, dirs, bytes);
            } else {
                char indent2[128];
                make_indent(depth + 1, indent2, sizeof(indent2));
                ESP_LOGI(TAG, "%s** path too long, skipping **", indent2);
            }
        } else {
            files++;
            bytes += (uint64_t)fno.fsize;

            // Decode FAT timestamp
            uint16_t d = fno.fdate, t = fno.ftime;
            uint16_t year  = (d >> 9) + 1980;
            uint8_t  month = (d >> 5) & 0x0F;
            uint8_t  day   = d & 0x1F;
            uint8_t  hour  = (t >> 11) & 0x1F;
            uint8_t  min   = (t >> 5) & 0x3F;
            uint8_t  sec   = (t & 0x1F) * 2;

            ESP_LOGI(TAG, "%s%s  %10lu  %04u-%02u-%02u %02u:%02u:%02u",
                     indent,
                     name,
                     (unsigned long)fno.fsize,
                     (unsigned)year, (unsigned)month, (unsigned)day,
                     (unsigned)hour, (unsigned)min, (unsigned)sec);
        }
    }

    f_closedir(&dir);
}

bool FileSystem::format(uint16_t /*au_kb*/) {
    // Work buffer: at least FF_MAX_SS bytes
    static uint8_t work[FF_MAX_SS];

    MKFS_PARM mk = {};
    mk.fmt     = FM_FAT | FM_FAT32;  // allow FAT or FAT32
    mk.n_fat   = 1;                  // 1 or 2 (1 is fine)
    mk.align   = 0;                  // auto (use disk's erase block via disk_ioctl)
    mk.n_root  = 0;                  // default (ignored on FAT32)
    mk.au_size = 0;                  // 0 = auto, in BYTES (per R0.14 API)

    FRESULT fr = f_mkfs("0:", &mk, work, sizeof(work));
    if (fr != FR_OK) {
        ESP_LOGI(TAG, "f_mkfs failed (%d)", (int)fr);
        return false;
    }
    return true;
}

void FileSystem::unmount()
{
    if (mounted_)
    {
        f_mount(nullptr, "0:", 0);
        mounted_ = false;
    }
    file_system_unregister_diskio(kDrv);
}

void FileSystem::handover_to_usb() { unmount(); }
bool FileSystem::reclaim_from_usb() { return setup(false); }

uint32_t FileSystem::sector_count_512() const
{
    // dhara_map_capacity() returns number of Dhara logical sectors (== pseudo pages)
    const uint32_t dhara_sectors = dhara_map_capacity(&map_);
    return dhara_sectors * (2048 / 512);
}

// ---- 512B sector cache helpers ------------------------------------------

bool FileSystem::load_page_to_cache(uint32_t lpage)
{
    if (cache_lpage_ == lpage)
        return true;
    if (!flush_cache_if_dirty())
        return false;

    dhara_error_t e = DHARA_E_NONE;
    int ret = dhara_map_read(&map_, lpage, page_cache_, &e);
    if (ret < 0 && e != DHARA_E_BAD_BLOCK)
    {
        // Unwritten page reads as 0xFF; supply that if read fails due to empty
        memset(page_cache_, 0xFF, 2048);
    }
    cache_lpage_ = lpage;
    cache_dirty_ = false;
    return true;
}

bool FileSystem::flush_cache_if_dirty()
{
    if (!cache_dirty_)
        return true;
    dhara_error_t e = DHARA_E_NONE;
    if (dhara_map_write(&map_, cache_lpage_, page_cache_, &e) < 0)
    {
        ESP_LOGI(TAG, "dhara_map_write fail (page %lu, err %d)", (unsigned long)cache_lpage_, (int)e);
        return false;
    }
    cache_dirty_ = false;
    return true;
}

void FileSystem::make_indent(int depth, char* out, size_t out_sz) {
    size_t n = (size_t)depth * 2;
    if (n >= out_sz) n = out_sz - 1;
    for (size_t i = 0; i < n; ++i) out[i] = ' ';
    out[n] = '\0';
}



// ---- diskio bridge (called from diskio_dhara.cpp C wrappers) -------------

DSTATUS FileSystem::disk_initialize() { return 0; }

DRESULT FileSystem::disk_read(uint8_t *buff, uint32_t lba, uint32_t count)
{
    const uint32_t spp = 2048 / 512;
    while (count--)
    {
        const uint32_t lpage = lba / spp;
        const uint32_t offset = (lba % spp) * 512u;
        if (!load_page_to_cache(lpage))
            return RES_ERROR;
        memcpy(buff, page_cache_ + offset, 512);
        buff += 512;
        lba++;
    }
    return RES_OK;
}

DRESULT FileSystem::disk_write(const uint8_t *buff, uint32_t lba, uint32_t count)
{
    const uint32_t spp = 2048 / 512;
    while (count--)
    {
        const uint32_t lpage = lba / spp;
        const uint32_t offset = (lba % spp) * 512u;
        if (!load_page_to_cache(lpage))
            return RES_ERROR;
        memcpy(page_cache_ + offset, buff, 512);
        cache_dirty_ = true;

        // If next sector is in a different page or this is the last sector, flush now
        if (count == 0 || ((lba + 1) / spp) != lpage)
        {
            if (!flush_cache_if_dirty())
                return RES_ERROR;
        }
        buff += 512;
        lba++;
    }
    return RES_OK;
}

DRESULT FileSystem::disk_ioctl(uint8_t cmd, void *buff)
{
    switch (cmd)
    {
    case CTRL_SYNC:
    {
        if (!flush_cache_if_dirty())
            return RES_ERROR;
        dhara_error_t e = DHARA_E_NONE;
        dhara_map_sync(&map_, &e);
        return (e == DHARA_E_NONE) ? RES_OK : RES_ERROR;
    }
    case GET_SECTOR_COUNT:
    {
        uint32_t *sc = reinterpret_cast<uint32_t *>(buff);
        *sc = sector_count_512();
        return RES_OK;
    }
    case GET_SECTOR_SIZE:
    {
        uint16_t *ss = reinterpret_cast<uint16_t *>(buff);
        *ss = 512; // USB MSC hosts expect 512B logical sectors
        return RES_OK;
    }
    case GET_BLOCK_SIZE:
    {
        // Erase block size in units of 512B sectors
        uint32_t *bs = reinterpret_cast<uint32_t *>(buff);
        const uint32_t erase_bytes = 64 * 2048;
        *bs = erase_bytes / 512u; // e.g., 64*2048/512 = 256
        return RES_OK;
    }
    case CTRL_TRIM:
    {
        // FatFs passes a LBA range list: DWORD[2] {start, end}
        DWORD *range = reinterpret_cast<DWORD *>(buff);
        const uint32_t start = range[0];
        const uint32_t end = range[1]; // inclusive
        const uint32_t spp = 2048 / 512;
        for (uint32_t lba = start; lba <= end; ++lba)
        {
            const uint32_t lpage = lba / spp;
            // Trim whole pages only once
            static uint32_t last = 0xFFFFFFFFu;
            if (lpage != last)
            {
                dhara_error_t e = DHARA_E_NONE;
                dhara_map_trim(&map_, lpage, &e);
                last = lpage;
            }
        }
        return RES_OK;
    }
    default:
        return RES_PARERR;
    }
}