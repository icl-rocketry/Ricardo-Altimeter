#include "SavedTelemetryPacket.h"

#include <librnp/rnp_packet.h>
#include <librnp/rnp_serializer.h>

#include <vector>



SavedTelemetryPacket::~SavedTelemetryPacket()
{};

SavedTelemetryPacket::SavedTelemetryPacket():
RnpPacket(0,
          101,
          size())
{};

SavedTelemetryPacket::SavedTelemetryPacket(const RnpPacketSerialized& packet):
RnpPacket(packet,size())
{
    getSerializer().deserialize(*this,packet.getBody());
};

void SavedTelemetryPacket::serialize(std::vector<uint8_t>& buf){
    RnpPacket::serialize(buf);
	size_t bufsize = buf.size();
	buf.resize(bufsize + size());
	std::memcpy(buf.data() + bufsize,getSerializer().serialize(*this).data(),size());
};