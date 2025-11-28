// #pragma once

// #include <librnp/rnp_packet.h>
// #include <librnp/rnp_serializer.h>
// #include "Config/forward_decl.h"

// #include <vector>

// class Logger {
//     public:

//         void esplogData(ForwardDecl_SystemClass& system);
//         std::string makeLogLine(ForwardDecl_SystemClass& system); // log line roughly 200 bytes (177 on test but want to leave margin)
//         std::string makePrintLine(ForwardDecl_SystemClass& system);
//         std::string makeHeaderLine();
//     private:

//         // 200 bytes per reading + header of like 100 bytes
//         // total usable space is 246,480,896 bytes
//         // 1.2 million readings
//         // assuming it needs to read for 6 hours
//         // 50 readings a second is 1.08 million readings
//         // appropriate polling rate is therefore 50Hz


//         //estimator output
//         float pd; // position NED (m) [North East Down]
//         float vd; // velocity NED (m/s)
//         float an, ae, ad; // acceleration NED (g's)

//         //orientation
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

// };


