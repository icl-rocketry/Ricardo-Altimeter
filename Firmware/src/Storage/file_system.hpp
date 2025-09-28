#pragma once
#include <stdint.h>
#include <stddef.h>
#include "ff.h"          // ChaN FatFs
#include "diskio.h"      // disk I/O glue
#include "nand_flash.hpp"
#include "translatedflash_ctx.hpp"
#include "diskio_dhara.hpp"
#include "esp_log.h"

class FileSystem {
    public:
        FileSystem(NANDFlash &nandflash);

        bool setup(bool mkfs_if_needed = true);
        void format_flash();
        void print_disk_space();
        void print_files(const char *start = "0:/");

        static FileSystem *instance();

        DSTATUS disk_initialize();
        DRESULT disk_read(uint8_t *buff, uint32_t lba, uint32_t count);
        DRESULT disk_write(const uint8_t *buff, uint32_t lba, uint32_t count);
        DRESULT disk_ioctl(uint8_t cmd, void *buff);

        void handover_to_usb();
        bool reclaim_from_usb();
        uint32_t sector_count_512() const;

    private:
        NANDFlash &nandflash_;
        TranslatedFlashCtx tf_;
        
        dhara_map map_; // dhara map structure
        uint8_t dhara_page_buf_[1 << 11]; // 2048B scratch (page-sized)
        
        bool load_page_to_cache(uint32_t lpage);
        bool flush_cache_if_dirty();
        
        FATFS fs_{};
        bool  mounted_ = false;
        
        // Cache for bridging 512B LBAs to Dhara logical sectors (e.g., 2048B)
        uint8_t  page_cache_[4096];
        uint32_t cache_lpage_ = 0xFFFFFFFFu;
        bool     cache_dirty_ = false;
        
        bool format(uint16_t au_kb = 16); // allocation unit (cluster) size in KB (e.g., 4, 8, 16, ...)
        void unmount();
        void list_dir_recursive(const char *path, int depth,
                            uint32_t &files, uint32_t &dirs, uint64_t &bytes);
        void make_indent(int depth, char* out, size_t out_sz);
        const char *TAG = "FS";

    };
