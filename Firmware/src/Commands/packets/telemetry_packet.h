// TelemetryPacket.hpp
#pragma once
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cinttypes>
#include "esp_timer.h"
#include "Sensors/sensor_structs.h"

class TelemetryPacket {
    public:
        TelemetryPacket() = default;
        void sendData(const SensorStructs::raw_measurements_t& m);
    private:

        static constexpr size_t kPayloadMax = 256;
        
        size_t make_raw_packet(const SensorStructs::raw_measurements_t& m, char* packet, size_t out_cap);
    
        // Convenience: write framed packet via USB CDC (printf)
        void send_packet(const char* packet);

        static uint8_t xor_checksum(const char* s, size_t n) {
            uint8_t c = 0;
            for (size_t i = 0; i < n; ++i) c ^= static_cast<uint8_t>(s[i]);
            return c;
        }
};
