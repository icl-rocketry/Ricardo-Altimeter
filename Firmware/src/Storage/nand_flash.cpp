#include "nand_flash.hpp"

NANDFlash::NANDFlash(SPIClass& spi , uint8_t cs, uint8_t wp, uint8_t hold) :
                m_as5f34g04snda(spi , cs, wp, hold) {};


NANDFlash::Error NANDFlash::setup() { 

    m_cache_ptr = (uint8_t *)malloc(2048 * sizeof(uint8_t));
    if (m_cache_ptr == nullptr) return NANDFlash::Error::CACHE_MEMORY_ALLOCATION;
    for (int i = 0; i < 2048; i++) {
        m_cache_ptr[i] = 0;
    }

    m_OOB_ptr = (uint8_t *)malloc(128 * sizeof(uint8_t));
    if (m_OOB_ptr == nullptr) return NANDFlash::Error::OOB_MEMORY_ALLOCATION;
    for (int i = 0; i < 128; i++) {
        m_OOB_ptr[i] = 0;
    }
    auto result = m_as5f34g04snda.setup();
    if (result != AS5F34G04SNDA::Error::SUCCESS) {
        Serial.println("NAND Flash IC Setup Failed " + String(static_cast<uint8_t>(result)));
        return NANDFlash::Error::SET_UP;
    }

    return NANDFlash::Error::SUCCESS;

}

NANDFlash::Error NANDFlash::read_page(uint16_t block_address, uint8_t page_address) {

    if (block_address >= 4096) return NANDFlash::Error::INVALID_BLOCK_ADDRESS;
    if (page_address >= 64) return NANDFlash::Error::INVALID_PAGE_ADDRESS;

    uint32_t shifted_block_address = static_cast<uint32_t>(block_address) << 6;
    uint32_t address = shifted_block_address | page_address;

    m_as5f34g04snda.read_to_cache(address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF); //reads from ic memory to ic cache

    NANDFlash::Error result = wait_for_processing(READ_TIMEOUT);
    if (result != NANDFlash::Error::SUCCESS) return result;

    m_as5f34g04snda.read_from_cache(m_cache_ptr, m_OOB_ptr); //reads from ic cash to esp cache

    return NANDFlash::Error::SUCCESS;

}

// write to the given block
NANDFlash::Error NANDFlash::write_page(uint16_t block_address, uint8_t page_address, const uint8_t* data) 
{ 
    if (block_address > 4096) return NANDFlash::Error::INVALID_BLOCK_ADDRESS;
    if (page_address > 64) return NANDFlash::Error::INVALID_PAGE_ADDRESS;

    uint32_t shifted_block_address = static_cast<uint32_t>(block_address) << 6;
    uint32_t address = shifted_block_address | page_address;

    m_as5f34g04snda.write_enable();

    std::memcpy(m_cache_ptr, data, 2048);
    
    m_as5f34g04snda.program_load(m_cache_ptr); 
    
    m_as5f34g04snda.program_execute(address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF); 

    NANDFlash::Error result = wait_for_processing(PROGRAM_TIMEOUT);

    m_as5f34g04snda.write_disable();

    return result;

}

NANDFlash::Error NANDFlash::internal_transfer(uint16_t src_block_address, uint8_t src_page_address, uint16_t dst_block_address, uint8_t dst_page_address) {

    if (src_block_address > 4096 || dst_block_address > 4096) return NANDFlash::Error::INVALID_BLOCK_ADDRESS;
    if (src_page_address > 64 || dst_page_address > 64) return NANDFlash::Error::INVALID_PAGE_ADDRESS;

    uint32_t shifted_src_block_address = static_cast<uint32_t>(src_block_address) << 6;
    uint32_t src_address = shifted_src_block_address | src_page_address;

    uint32_t shifted_dst_block_address = static_cast<uint32_t>(dst_block_address) << 6;
    uint32_t dst_address = shifted_dst_block_address | dst_page_address;

    m_as5f34g04snda.read_to_cache(src_address >> 16 & 0xFF, src_address >> 8 & 0xFF, src_address & 0xFF);
    
    NANDFlash::Error result = wait_for_processing(READ_TIMEOUT);
    if (result != NANDFlash::Error::SUCCESS) return result;

    m_as5f34g04snda.write_enable();

    m_as5f34g04snda.program_execute(dst_address >> 16 & 0xFF, dst_address >> 8 & 0xFF, dst_address & 0xFF);
        
    result = wait_for_processing(PROGRAM_TIMEOUT);

    m_as5f34g04snda.write_disable();

    return result;
}

NANDFlash::Error NANDFlash::erase_block(uint16_t block_address) { // block erase also deletes the bad block marks fyi

    if (block_address > 4096) return NANDFlash::Error::INVALID_BLOCK_ADDRESS;

    uint32_t shifted_block_address = static_cast<uint32_t>(block_address) << 6;
    uint32_t address = shifted_block_address;

    m_as5f34g04snda.write_enable();

    m_as5f34g04snda.block_erase(address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF);

    NANDFlash::Error result = wait_for_processing(ERASE_TIMEOUT);

    m_as5f34g04snda.write_disable();

    return result;
}

// returns true if block is bad
bool NANDFlash::is_block_bad(uint16_t block_address) {
    read_page(block_address, 0);
    return m_OOB_ptr[0] != 0xFF;
}

void NANDFlash::mark_bad_block(uint16_t block_address) {
    if (block_address > 4096) return;

    uint32_t address = static_cast<uint32_t>(block_address) << 6;

    m_as5f34g04snda.write_enable();

    m_as5f34g04snda.program_load_bad_block_mark(); 
    
    m_as5f34g04snda.program_execute(address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF); 

    wait_for_processing(PROGRAM_TIMEOUT);

    m_as5f34g04snda.write_disable();
}

NANDFlash::Error NANDFlash::wait_for_processing(unsigned long timeout_us) {
    const uint32_t start = micros();  

    for (;;) { // short hand for while (true)
        const uint8_t status = m_as5f34g04snda.get_status_feature();

        if ((status & 0x01u) == 0u) { // if not busy

            if (status & (1u << 2)) return NANDFlash::Error::ERASE_FAIL;

            if (status & (1u << 3)) return NANDFlash::Error::PROGRAM_FAIL;

            if ((status & (1u << 5)) && !(status & (1u << 4))) return NANDFlash::Error::ECC_FAIL; // uncorrectable ECC
            
            return NANDFlash::Error::SUCCESS;
        }

        // Wrap-safe timeout check
        if ((uint32_t)(micros() - start) > (uint32_t)timeout_us) {
            return NANDFlash::Error::READ_PAGE_TIMEOUT; // or a generic OP_TIMEOUT
        }

        delayMicroseconds(10);
    }
}

void NANDFlash::print_cache() {
    Serial.print("Cache: ");
    for (int i = 0; i < 2048; i++) {
        Serial.print(m_cache_ptr[i]);
        Serial.print(" ");
    }
    Serial.println();
};

void NANDFlash::print_cache_short() {
    Serial.print("Cache: ");
    for (int i = 0; i < 50; i++) {
        Serial.print(m_cache_ptr[i]);
        Serial.print(" ");
    }
    Serial.println();
};

void NANDFlash::print_OOB() {
    Serial.print("OOB: ");
    for (int i = 0; i < 128; i++) {
        Serial.print(m_OOB_ptr[i]);
        Serial.print(" ");
    }
    Serial.println();
};

int NANDFlash::debug() {
    return static_cast<uint8_t>(m_as5f34g04snda.setup());
}
int NANDFlash::debug1() {
    return static_cast<uint8_t>(m_as5f34g04snda.get_block_lock_feature());
}