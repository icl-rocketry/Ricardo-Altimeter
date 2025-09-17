// translatedflash_ctx.hpp
#pragma once
#include <cstddef> // offsetof
#include <cstdint>
#include "nand_flash.hpp"

extern "C" {
#include "dhara/nand.h"
#include "dhara/error.h"
#include "dhara/map.h"
}

struct TranslatedFlashCtx {
	dhara_nand dn;	  // what Dhara sees
	NANDFlash *flash; // your driver

	void init(NANDFlash &drv,
			  uint8_t log2_page_size, // 11 for 2KiB
			  uint8_t log2_ppb,		  // 6 for 64 pages/block
			  uint32_t num_blocks)	  // e.g. 4096
	{
		dn.log2_page_size = log2_page_size;
		dn.log2_ppb = log2_ppb;
		dn.num_blocks = num_blocks;
		flash = &drv;
	}
};

// container-of: get back the outer ctx from &ctx.dn
inline TranslatedFlashCtx *ctx_from_dn(const dhara_nand *n)
{
	return (TranslatedFlashCtx *)((const char *)n - offsetof(TranslatedFlashCtx, dn));
}

// // auto result = nandflash.setup();
// // if (result != NANDFlash::Error::SUCCESS) {
// //     Serial.println("NAND Flash Setup Failed " + String(static_cast<uint8_t>(result)));
// // } else {
// //     Serial.println("NAND Flash Setup Success");
// // }

// // tf_.init(nandflash, /*log2_page_size*/11, /*log2_ppb*/6, /*num_blocks*/4096);

// // dhara_map_init(&map_, &tf_.dn, dhara_page_buf_, /*gc_ratio=*/1);

// // const size_t PSZ = 1u << tf_.dn.log2_page_size;   // 2048
// // static uint8_t page[1 << 11];                     // 2 KiB
// // memset(page, 0, PSZ);

// // dhara_error_t e = DHARA_E_NONE;

// // if (dhara_map_resume(&map_, &e) == 0) {
// //     Serial.println("Dhara resume OK");
// // } else {
// //     Serial.printf("Resume failed (%d). Formatting via write+sync…\n", (int)e);

// //     if (dhara_map_write(&map_, /*lba*/0, page, &e) < 0) {
// //         Serial.printf("format write failed: %d\n", (int)e);
// //     } else if (dhara_map_sync(&map_, &e) < 0) {
// //         Serial.printf("format sync failed: %d\n", (int)e);
// //     } else {
// //         if (dhara_map_resume(&map_, &e) == 0)
// //         Serial.println("Resume OK after write+sync");
// //         else
// //         Serial.printf("Resume still failed: %d\n", (int)e);
// //     }
// // }

// // gave

// // NAND Flash Setup Success
// // Is Bad Block 0
// // Read Page 15
// // Is Bad Block 1
// // Read Page 79
// // Is Bad Block 2
// // Read Page 143
// // Is Bad Block 3
// // Read Page 207
// // Is Bad Block 4
// // Read Page 271
// // Is Bad Block 5
// // Read Page 335
// // Is Bad Block 6
// // Read Page 399
// // Is Bad Block 7
// // Read Page 463
// // Resume failed (3). Formatting via write+sync…
// // Is Bad Block 0
// // Erase Block 0
// // Program Page 0
// // Copy Page 0 to 1
// // Copy Page 1 to 2
// // Copy Page 2 to 3
// // Copy Page 3 to 4
// // Copy Page 4 to 5
// // Copy Page 5 to 6
// // Copy Page 6 to 7
// // Copy Page 7 to 8
// // Copy Page 8 to 9
// // Copy Page 9 to 10
// // Copy Page 10 to 11
// // Copy Page 11 to 12
// // Copy Page 12 to 13
// // Copy Page 13 to 14
// // Program Page 15
// // Is Bad Block 0
// // Read Page 15
// // Is Bad Block 2047
// // Read Page 131023
// // Is Bad Block 2048
// // Read Page 131087
// // Is Bad Block 2049
// // Read Page 131151
// // Is Bad Block 2050
// // Read Page 131215
// // Is Bad Block 2051
// // Read Page 131279
// // Is Bad Block 2052
// // Read Page 131343
// // Is Bad Block 2053
// // Read Page 131407
// // Is Bad Block 2054
// // Read Page 131471
// // Is Bad Block 1023
// // Read Page 65487
// // Is Bad Block 1024
// // Read Page 65551
// // Is Bad Block 1025
// // Read Page 65615
// // Is Bad Block 1026
// // Read Page 65679
// // Is Bad Block 1027
// // Read Page 65743
// // Is Bad Block 1028
// // Read Page 65807
// // Is Bad Block 1029
// // Read Page 65871
// // Is Bad Block 1030
// // Read Page 65935
// // Is Bad Block 511
// // Read Page 32719
// // Is Bad Block 512
// // Read Page 32783
// // Is Bad Block 513
// // Read Page 32847
// // Is Bad Block 514
// // Read Page 32911
// // Is Bad Block 515
// // Read Page 32975
// // Is Bad Block 516
// // Read Page 33039
// // Is Bad Block 517
// // Read Page 33103
// // Is Bad Block 518
// // Read Page 33167
// // Is Bad Block 255
// // Read Page 16335
// // Is Bad Block 256
// // Read Page 16399
// // Is Bad Block 257
// // Read Page 16463
// // Is Bad Block 258
// // Read Page 16527
// // Is Bad Block 259
// // Read Page 16591
// // Is Bad Block 260
// // Read Page 16655
// // Is Bad Block 261
// // Read Page 16719
// // Is Bad Block 262
// // Read Page 16783
// // Is Bad Block 127
// // Read Page 8143
// // Is Bad Block 128
// // Read Page 8207
// // Is Bad Block 129
// // Read Page 8271
// // Is Bad Block 130
// // Read Page 8335
// // Is Bad Block 131
// // Read Page 8399
// // Is Bad Block 132
// // Read Page 8463
// // Is Bad Block 133
// // Read Page 8527
// // Is Bad Block 134
// // Read Page 8591
// // Is Bad Block 63
// // Read Page 4047
// // Is Bad Block 64
// // Read Page 4111
// // Is Bad Block 65
// // Read Page 4175
// // Is Bad Block 66
// // Read Page 4239
// // Is Bad Block 67
// // Read Page 4303
// // Is Bad Block 68
// // Read Page 4367
// // Is Bad Block 69
// // Read Page 4431
// // Is Bad Block 70
// // Read Page 4495
// // Is Bad Block 31
// // Read Page 1999
// // Is Bad Block 32
// // Read Page 2063
// // Is Bad Block 33
// // Read Page 2127
// // Is Bad Block 34
// // Read Page 2191
// // Is Bad Block 35
// // Read Page 2255
// // Is Bad Block 36
// // Read Page 2319
// // Is Bad Block 37
// // Read Page 2383
// // Is Bad Block 38
// // Read Page 2447
// // Is Bad Block 15
// // Read Page 975
// // Is Bad Block 16
// // Read Page 1039
// // Is Bad Block 17
// // Read Page 1103
// // Is Bad Block 18
// // Read Page 1167
// // Is Bad Block 19
// // Read Page 1231
// // Is Bad Block 20
// // Read Page 1295
// // Is Bad Block 21
// // Read Page 1359
// // Is Bad Block 22
// // Read Page 1423
// // Is Bad Block 7
// // Read Page 463
// // Is Bad Block 8
// // Read Page 527
// // Is Bad Block 9
// // Read Page 591
// // Is Bad Block 10
// // Read Page 655
// // Is Bad Block 11
// // Read Page 719
// // Is Bad Block 12
// // Read Page 783
// // Is Bad Block 13
// // Read Page 847
// // Is Bad Block 14
// // Read Page 911
// // Is Bad Block 3
// // Read Page 207
// // Is Bad Block 4
// // Read Page 271
// // Is Bad Block 5
// // ␛[0;31mE (9123) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:␛[0m
// // ␛[0;31mE (9123) task_wdt:  - IDLE (CPU 0)␛[0m
// // ␛[0;31mE (9123) task_wdt: Tasks currently running:␛[0m
// // ␛[0;31mE (9123) task_wdt: CPU 0: loopTask␛[0m
// // ␛[0;31mE (9123) task_wdt: Print CPU 0 (current core) backtrace␛[0m

// // Backtrace: 0x4201637E:0x3FC933D0 0x40376BFE:0x3FC933F0 0x4200CB91:0x3FCEB920 0x4200BEE0:0x3FCEB940 0x42004205:0x3FCEB960 0x420045E5:0x3FCEB980 0x42004604:0x3FCEB9A0 0x420047FE:0x3FCEB9C0 0x42009B89:0x3FCEBA20 0x42009C07:0x3FCEBA40 0x4200A25E:0x3FCEBA70 0x420098ED:0x3FCEBAA0 0x42008D85:0x3FCEBAC0 0x420062E2:0x3FCEBB10 0x420066DB:0x3FCEBE00 0x4037DABD:0x3FCEBE20

// // Read Page 335
// // Is Bad Block 6
// // Read Page 399
// // Is Bad Block 7
// // Read Page 463
// // Is Bad Block 8
// // Read Page 527
// // Is Bad Block 9
// // Read Page 591
// // Is Bad Block 10
// // Read Page 655
// // Is Bad Block 1
// // Read Page 79
// // Is Bad Block 2
// // Read Page 143
// // Is Bad Block 3
// // Read Page 207
// // Is Bad Block 4
// // Read Page 271
// // Is Bad Block 5
// // Read Page 335
// // Is Bad Block 6
// // Read Page 399
// // Is Bad Block 7
// // Read Page 463
// // Is Bad Block 8
// // Read Page 527
// // Is Bad Block 0
// // Read Page 15
// // Is Bad Block 1
// // Read Page 79
// // Is Bad Block 2
// // Read Page 143
// // Is Bad Block 3
// // Read Page 207
// // Is Bad Block 4
// // Read Page 271
// // Is Bad Block 5
// // Read Page 335
// // Is Bad Block 6
// // Read Page 399
// // Is Bad Block 7
// // Read Page 463
// // Is Bad Block 8
// // Read Page 527
// // Is Free Page 16
// // Is Free Block 0 Page 16
// // Is Free Page 17
// // Is Free Block 0 Page 17
// // Is Free Page 18
// // Is Free Block 0 Page 18
// // Is Free Page 19
// // Is Free Block 0 Page 19
// // Is Free Page 20
// // Is Free Block 0 Page 20
// // Is Free Page 21
// // Is Free Block 0 Page 21
// // Is Free Page 22
// // Is Free Block 0 Page 22
// // Is Free Page 23
// // Is Free Block 0 Page 23
// // Is Free Page 24
// // Is Free Block 0 Page 24
// // Is Free Page 25
// // Is Free Block 0 Page 25
// // Is Free Page 26
// // Is Free Block 0 Page 26
// // Is Free Page 27
// // Is Free Block 0 Page 27
// // Is Free Page 28
// // Is Free Block 0 Page 28
// // Is Free Page 29
// // Is Free Block 0 Page 29
// // Is Free Page 30
// // Is Free Block 0 Page 30
// // Is Free Page 31
// // Is Free Block 0 Page 31
// // Is Free Page 0
// // Is Free Block 0 Page 0
// // Not Free, byte 0 = 0 in location 0,0
// // Is Free Page 16
// // Is Free Block 0 Page 16
// // Is Free Page 17
// // Is Free Block 0 Page 17
// // Is Free Page 18
// // Is Free Block 0 Page 18
// // Is Free Page 19
// // Is Free Block 0 Page 19
// // Is Free Page 20
// // Is Free Block 0 Page 20
// // Is Free Page 21
// // Is Free Block 0 Page 21
// // Is Free Page 22
// // Is Free Block 0 Page 22
// // Is Free Page 23
// // Is Free Block 0 Page 23
// // Is Free Page 24
// // Is Free Block 0 Page 24
// // Is Free Page 25
// // Is Free Block 0 Page 25
// // Is Free Page 26
// // Is Free Block 0 Page 26
// // Is Free Page 27
// // Is Free Block 0 Page 27
// // Is Free Page 28
// // Is Free Block 0 Page 28
// // Is Free Page 29
// // Is Free Block 0 Page 29
// // Is Free Page 30
// // Is Free Block 0 Page 30
// // Is Free Page 31
// // Is Free Block 0 Page 31
// // Read Page 15
// // Is Free Page 15
// // Is Free Block 0 Page 15
// // Not Free, byte 0 = 44 in location 0,15
// // Is Free Page 31
// // Is Free Block 0 Page 31
// // Is Free Page 30
// // Is Free Block 0 Page 30
// // Is Free Page 29
// // Is Free Block 0 Page 29
// // Is Free Page 28
// // Is Free Block 0 Page 28
// // Is Free Page 27
// // Is Free Block 0 Page 27
// // Is Free Page 26
// // Is Free Block 0 Page 26
// // Is Free Page 25
// // Is Free Block 0 Page 25
// // Is Free Page 24
// // Is Free Block 0 Page 24
// // Is Free Page 23
// // Is Free Block 0 Page 23
// // Is Free Page 22
// // Is Free Block 0 Page 22
// // Is Free Page 21
// // Is Free Block 0 Page 21
// // Is Free Page 20
// // Is Free Block 0 Page 20
// // Is Free Page 19
// // Is Free Block 0 Page 19
// // Is Free Page 18
// // Is Free Block 0 Page 18
// // Is Free Page 17
// // Is Free Block 0 Page 17
// // Is Free Page 16
// // Is Free Block 0 Page 16
// // Resume OK after write+sync
// // SYS_LOG:[12026] -> System Setup Complete