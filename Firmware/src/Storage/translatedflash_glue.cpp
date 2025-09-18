#include "translatedflash_ctx.hpp"
#include <cstring>

extern "C" {

    int dhara_nand_is_bad(const struct dhara_nand *n, dhara_block_t b) {
        delay(1); // yield
        // Serial.println("Is Bad Block " + String(b));
        auto *ctx = ctx_from_dn(n);
        int ret = ctx->flash->is_block_bad((uint16_t)b);
        // if (ret == 1) Serial.println("Block " + String(b) + " is bad");
        return ret ? 1 : 0;
    }

    void dhara_nand_mark_bad(const struct dhara_nand *n, dhara_block_t b) {
        delay(1); // yield
        // Serial.println("Mark Bad Block " + String(b));
        auto *ctx = ctx_from_dn(n);
        ctx->flash->mark_bad_block((uint16_t)b);
    }

    int dhara_nand_erase(const struct dhara_nand *n, dhara_block_t b, dhara_error_t *err) {
        delay(1); // yield
        // Serial.println("Erase Block " + String(b));
        auto *ctx = ctx_from_dn(n);
        auto result = ctx->flash->erase_block((uint16_t)b);
        if (result == NANDFlash::Error::SUCCESS) return 0;
        // Serial.println("Erase block " + String(b) + " failed: " + String(static_cast<uint8_t>(result)));
        if (err) *err = DHARA_E_BAD_BLOCK;
        return -1;
    }

    int dhara_nand_prog(const struct dhara_nand *n, dhara_page_t p,
                        const uint8_t *data, dhara_error_t *err) {
        delay(1); // yield
        // Serial.println("Program Page " + String(p));
        auto *ctx = ctx_from_dn(n);
        const uint16_t block = (uint16_t)(p >> n->log2_ppb);
        const uint8_t  page  = (uint8_t)(p & ((1u << n->log2_ppb) - 1));
        auto result = ctx->flash->write_page(block, page, data); // const* overload
        if (result == NANDFlash::Error::SUCCESS) return 0;
        // Serial.println("Program block " + String(block) + " page " + String(page) + " failed: " + String(static_cast<uint8_t>(result)));
        if (err) *err = DHARA_E_BAD_BLOCK;
        return -1;
    }

    int dhara_nand_is_free(const struct dhara_nand *n, dhara_page_t p) {
        delay(1); // yield
        // Serial.println("Is Free Page " + String(p));
        auto *ctx = ctx_from_dn(n);
        const uint16_t block = (uint16_t)(p >> n->log2_ppb);
        const uint8_t  page  = (uint8_t)(p & ((1u << n->log2_ppb) - 1));
        // Serial.println("Is Free Block " + String(block) + " Page " + String(page));
        if (ctx->flash->read_page(block, page) != NANDFlash::Error::SUCCESS) return 0;
        const uint8_t *d = ctx->flash->m_cache_ptr;
        for (int i = 0; i < 2048; ++i) if (d[i] != 0xFF) {
            // Serial.println("Not Free, byte " + String(i) + " = " + String(d[i], HEX) + " in location " + String(block) + "," + String(page));
            return 0;
        } 
        return 1;
    }

    int dhara_nand_read(const struct dhara_nand *n, dhara_page_t p,
                    size_t offset, size_t length, uint8_t *data,
                    dhara_error_t *err) {
        delay(1); // yield
        // Serial.println("Read Page " + String(p));
        auto *ctx = ctx_from_dn(n);
        if (offset + length > 2048) { if (err) *err = DHARA_E_BAD_BLOCK; return -1; }
        const uint16_t block = (uint16_t)(p >> n->log2_ppb);
        const uint8_t  page  = (uint8_t)(p & ((1u << n->log2_ppb) - 1));
        auto result = ctx->flash->read_page(block, page);
        if (result != NANDFlash::Error::SUCCESS) {
            if (err) *err = (result == NANDFlash::Error::ECC_FAIL) ? DHARA_E_ECC : DHARA_E_BAD_BLOCK;
            // Serial.println("Read block " + String(block) + " page " + String(page) + " failed: " + String(static_cast<uint8_t>(result)));
            return -1;
        }
        std::memcpy(data, ctx->flash->m_cache_ptr + offset, length);

        return 0;
    }
    
    int dhara_nand_copy(const struct dhara_nand* n, dhara_page_t src, dhara_page_t dst,
                        dhara_error_t* err) {
        delay(1); // yield
        // Serial.println("Copy Page " + String(src) + " to " + String(dst));
        auto* ctx = ctx_from_dn(n);
        const uint16_t src_block = (uint16_t)(src >> n->log2_ppb);
        const uint8_t  src_page  = (uint8_t)(src & ((1u << n->log2_ppb) - 1));
        const uint16_t dst_block = (uint16_t)(dst >> n->log2_ppb);
        const uint8_t  dst_page  = (uint8_t)(dst & ((1u << n->log2_ppb) - 1));
        auto result = ctx->flash->internal_transfer(src_block, src_page, dst_block, dst_page);
        if (result == NANDFlash::Error::SUCCESS) return 0;
        if (err) *err = DHARA_E_BAD_BLOCK;
        // Serial.println("Copy src " + String(src_block) + "," + String(src_page) + " to dst " + String(dst_block) + "," + String(dst_page) + " failed: " + String(static_cast<uint8_t>(result)));
        return -1;
    }

}