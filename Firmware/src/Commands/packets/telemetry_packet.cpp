#include "telemetry_packet.h"

void TelemetryPacket::sendData(const SensorStructs::raw_measurements_t& m) {
    char packet[256];
    size_t plen = make_raw_packet(m, packet, sizeof(packet));
    if (plen > 0) {
        send_packet(packet);
    }
}

size_t TelemetryPacket::make_raw_packet(const SensorStructs::raw_measurements_t& m, char* packet, size_t out_cap) {
    char payload[kPayloadMax];
    double t_s = esp_timer_get_time() / 1e6;

    int plen = std::snprintf(payload, sizeof(payload),
                                "RAW,%.6f,"
                                "%.6f,%.6f,%.6f,"            // ax, ay, az
                                "%.6f,%.6f,%.6f,%.3f,"       // gx, gy, gz, temp
                                "%.6f,%.6f,%.6f,"            // accel_only ax,ay,az
                                "%.6f,%.6f,%.6f,%.3f,"       // mag mx,my,mz,temp
                                "%.3f,%.3f,%.3f,"            // baro alt,temp,press
                                "%" PRIu16 ",%" PRIu16 ","   // logic mV, %
                                "%" PRIu64,                  // system_time
                                t_s,
                                m.accelgyro.ax, m.accelgyro.ay, m.accelgyro.az,
                                m.accelgyro.gx, m.accelgyro.gy, m.accelgyro.gz, m.accelgyro.temp,
                                m.accel.ax, m.accel.ay, m.accel.az,
                                m.mag.mx, m.mag.my, m.mag.mz, m.mag.temp,
                                m.baro.alt, m.baro.temp, m.baro.press,
                                m.logicrail.volt, m.logicrail.percent,
                                m.system_time);
    if (plen <= 0 || static_cast<size_t>(plen) >= sizeof(payload)) return 0;

    uint8_t cs = xor_checksum(payload, static_cast<size_t>(plen));
    int w = std::snprintf(packet, out_cap, "$%s*%02X\r\n", payload, cs);
    if (w <= 0 || static_cast<size_t>(w) >= out_cap) return 0;
    return static_cast<size_t>(w);
}

void TelemetryPacket::send_packet(const char* packet) {
    std::printf("%s", packet);
}