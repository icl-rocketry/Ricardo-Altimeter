// #pragma once

// #include <librnp/rnp_packet.h>
// #include <librnp/rnp_serializer.h>

// #include "Config/forward_decl.h"

// #include <vector>

// class Logger {
//     public:

//         void esplogData(System& system);
//     private:

//         //packet header
//         //PacketHeader header{static_cast<uint8_t>(packet::TELEMETRY), packet_size()};
//         //estimator output
//         float pn, pe, pd; // position NED (m) [North East Down]
//         float vn, ve, vd; // velocity NED (m/s)
//         float an, ae, ad; // acceleration NED (g's)
//         //orientation
//         float roll,pitch,yaw; // orientation degrees
//         float q0,q1,q2,q3; //quaternion representation
//         //imu
//         float ax, ay, az; // acceleration (g's)
//         float h_ax,h_ay,h_az;// high g accel (g's)
//         float gx, gy, gz; // angular rates (deg/s)
//         float mx, my, mz;// magnetometer (uT)
//         //barometer
//         float baro_temp, baro_press,baro_alt;
//         //battery
//         uint16_t batt_voltage,batt_percent;
//         //system details
//         uint32_t system_status;
//         uint64_t system_time;

// };


