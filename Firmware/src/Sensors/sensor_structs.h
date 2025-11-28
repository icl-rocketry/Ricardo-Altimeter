#pragma once
/*
definition of structs used within sensor classes
*/

#include <Eigen/Core>
#include <Eigen/Eigen>
#include <Eigen/Geometry>

namespace SensorStructs
{

    struct ACCELGYRO_6AXIS_t{
        float ax; 
        float ay;
        float az;
        float gx;
        float gy;
        float gz;

        float temp;
    };

    struct ACCEL_3AXIS_t{
        float ax;
        float ay;
        float az;
    };
    struct MAG_3AXIS_t{
        float mx;
        float my;
        float mz;

        float temp;
    };
    struct BARO_t{
         // BARO RAW DATA
        float alt;
        float temp;
        float press;
    };
    
    struct ADC_V_RAIL_t{
        /**
         * @brief Voltage in mV
         * 
         */
        uint16_t volt;
        
        /**
         * @brief Percentage in reference to max voltage expeceted 
         * 
         */
        uint16_t percent;
    };

    struct raw_measurements_t
    {
        ACCELGYRO_6AXIS_t accelgyro;
        ACCEL_3AXIS_t accel;
        MAG_3AXIS_t mag;
        BARO_t baro;
        ADC_V_RAIL_t logicrail;

        uint64_t system_time;
    };

    struct state_t
    {

        Eigen::Quaternionf orientation; //(quaternion)
        Eigen::Vector3f eulerAngles;    //(deg) (roll pitch yaw)
        Eigen::Vector3f position;       //(m) relative to callibration site (NED)
        Eigen::Vector3f velocity;       //(ms-1) (NED)
        Eigen::Vector3f acceleration;   //(ms-2) (NED)
        Eigen::Vector3f angularRates;   //(deg/s)

        float  baro_ref_alt;
        /**
         * @brief Estimator state -> maybe change this to be a bitfield lol
         * 
         * 
            NOMINAL = 0
            PARTIAL_NO_IMU = 1
            PARTIAL_NO_IMU_NO_GPS = 2
            PARTIAL_NO_IMU_NO_BARO = 3
            PARTIAL_NO_MAG = 4
            PARTIAL_NO_GPS = 5
            PARTIAL_NO_GPS_NO_BARO = 6
            PARTIAL_NO_BARO = 7
            NO_HOME = 8
            NOSOLUTION = 9
         * 
         */
        uint8_t estimator_state;
    };

}
