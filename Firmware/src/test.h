// #include "esp_log.h"

// static const char* TAG = "PKT";

// // For any RnpPacket (e.g., MessagePacket_Base, BasicDataPacket, etc.)
// void dumpTx(const RnpPacket& pkt) {
//     std::vector<uint8_t> buf;
//     const_cast<RnpPacket&>(pkt).serialize(buf);  // serialize header+body
//     ESP_LOG_BUFFER_HEX_LEVEL(TAG, buf.data(), buf.size(), ESP_LOG_DEBUG);
// }

// // For an already-serialized packet
// void dumpRx(const RnpPacketSerialized& spkt) {
//     // Ensures header bytes in spkt.packet reflect spkt.header if you modified it
//     const_cast<RnpPacketSerialized&>(spkt).reserializeHeader();
//     ESP_LOG_BUFFER_HEX_LEVEL(TAG, spkt.packet.data(), spkt.packet.size(), ESP_LOG_DEBUG);
// }