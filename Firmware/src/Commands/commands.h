#pragma once

#include <stdint.h>

#include "esp_log.h"

#include <librnp/rnp_packet.h>

#include "Config/forward_decl.h"
#include "packets/telemetry_packet.h"

#include "States/msc.h"

namespace Commands{
    
    void FreeRamCommand(ForwardDecl_SystemClass& system, const RnpPacketSerialized& packet);
    void TelemetryCommand(ForwardDecl_SystemClass& system, const RnpPacketSerialized& packet);
    // void EnterMSCCommand(ForwardDecl_SystemClass& system, const RnpPacketSerialized& packet);
}