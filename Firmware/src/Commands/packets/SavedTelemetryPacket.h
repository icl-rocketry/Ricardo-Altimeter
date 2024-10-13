#pragma once

#include <librnp/rnp_packet.h>
#include <librnp/rnp_serializer.h>

#include <vector>

class SavedTelemetryPacket : public RnpPacket{
    private:
    //serializer framework
        static constexpr auto getSerializer()
        {
            auto ret = RnpSerializer(
                &SavedTelemetryPacket::pn,
                &SavedTelemetryPacket::pe,
                &SavedTelemetryPacket::pd,
                &SavedTelemetryPacket::vn,
                &SavedTelemetryPacket::ve,
                &SavedTelemetryPacket::vd,
                &SavedTelemetryPacket::an,
                &SavedTelemetryPacket::ae,
                &SavedTelemetryPacket::ad,
                &SavedTelemetryPacket::roll,
                &SavedTelemetryPacket::pitch,
                &SavedTelemetryPacket::yaw,
                &SavedTelemetryPacket::q0,
                &SavedTelemetryPacket::q1,
                &SavedTelemetryPacket::q2,
                &SavedTelemetryPacket::q3,
                &SavedTelemetryPacket::ax,
                &SavedTelemetryPacket::ay,
                &SavedTelemetryPacket::az,
                &SavedTelemetryPacket::h_ax,
                &SavedTelemetryPacket::h_ay,
                &SavedTelemetryPacket::h_az,
                &SavedTelemetryPacket::gx,
                &SavedTelemetryPacket::gy,
                &SavedTelemetryPacket::gz,
                &SavedTelemetryPacket::mx,
                &SavedTelemetryPacket::my,
                &SavedTelemetryPacket::mz,
                &SavedTelemetryPacket::baro_temp,
                &SavedTelemetryPacket::baro_press,
                &SavedTelemetryPacket::baro_alt,
                &SavedTelemetryPacket::batt_voltage,
                &SavedTelemetryPacket::batt_percent,
                &SavedTelemetryPacket::system_status,
                &SavedTelemetryPacket::system_time,
                &SavedTelemetryPacket::block,
                &SavedTelemetryPacket::page,
                &SavedTelemetryPacket::telem_num
            );
            return ret;
        }
        
    public:
        ~SavedTelemetryPacket();

        SavedTelemetryPacket();
        /**
         * @brief Deserialize Telemetry Packet
         * 
         * @param data 
         */
        SavedTelemetryPacket(const RnpPacketSerialized& packet);

        /**
         * @brief Serialize Telemetry Packet
         * 
         * @param buf 
         */
        void serialize(std::vector<uint8_t>& buf) override;

        
        //packet header
        //PacketHeader header{static_cast<uint8_t>(packet::TELEMETRY), packet_size()};
        //estimator output
        float pn, pe, pd; // position NED (m) [North East Down]
        float vn, ve, vd; // velocity NED (m/s)
        float an, ae, ad; // acceleration NED (g's)
        //orientation
        float roll,pitch,yaw; // orientation degrees
        float q0,q1,q2,q3; //quaternion representation
        //gps
        float lat,lng;
        long alt;//in mm
        uint8_t sat;
        //imu
        float ax, ay, az; // acceleration (g's)
        float h_ax,h_ay,h_az;// high g accel (g's)
        float gx, gy, gz; // angular rates (deg/s)
        float mx, my, mz;// magnetometer (uT)
        //barometer
        float baro_temp, baro_press,baro_alt;
        //battery
        uint16_t batt_voltage,batt_percent;
        //launch site
        float launch_lat,launch_lng;
        long launch_alt;
        //system details
        uint32_t system_status;
        uint64_t system_time;
        //radio details
        int16_t rssi; 
        float snr;

        float block;
        float page;
        float telem_num;

        static constexpr size_t size(){
            return getSerializer().member_size();
        }

};


