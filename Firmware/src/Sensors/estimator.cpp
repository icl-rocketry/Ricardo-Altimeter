#include "estimator.h"

#include <string>
#include "math.h"

#include "Eigen/Eigen"

#include <ArduinoJson.h>
#include <libriccore/riccorelogging.h>

#include "Config/types.h"
#include "Config/systemflags_config.h"

#include "sensors.h"



Estimator::Estimator(Types::CoreTypes::SystemStatus_t &systemstatus) : _systemstatus(systemstatus),
                                                   update_frequency(2000), // 500Hz update with 2000
                                                   _homeSet(false),
                                                   madgwick(0.5f, 0.005f) // beta | gyroscope sample time step (s)
                                                   {};

void Estimator::setup()
{
   // update board orientation this is applied when converthing back to sensor frame where the orientaiton of sensor matters
   // upside down should be retireved from config file

   localizationkf.reset();
};

void Estimator::update(const SensorStructs::raw_measurements_t &raw_sensors)
{

   unsigned long dt = (unsigned long)(micros() - last_update); // explictly casting to prevent micros() overflow cuasing issues

   if (dt > update_frequency)
   {
      last_update = micros();                   // update last_update
      float dt_seconds = float(dt) * 0.000001F; // conversion to seconds

      updateOrientation(raw_sensors.accelgyro.gx, raw_sensors.accelgyro.gy, raw_sensors.accelgyro.gz,
                        raw_sensors.accelgyro.ax, raw_sensors.accelgyro.ay, raw_sensors.accelgyro.az,
                        // raw_sensors.mag.mx, raw_sensors.mag.my, raw_sensors.mag.mz,
                        dt_seconds);

      // transform angular rates from body frame to earth frame
      updateAngularRates(raw_sensors.accelgyro.gx, raw_sensors.accelgyro.gy, raw_sensors.accelgyro.gz);
      localizationkf.accelUpdate(getLinearAcceleration(raw_sensors.accelgyro.ax, raw_sensors.accelgyro.ay, raw_sensors.accelgyro.az) * g);
      
      if (!_homeSet)
      {
         Serial.println("Home not set, Localization not avaliable");
         return;
      }

      localizationkf.baroUpdate(raw_sensors.baro.alt - state.baro_ref_alt);

      predictLocalizationKF(dt_seconds);

   }
};

void Estimator::setHome(const SensorStructs::raw_measurements_t &raw_sensors)
{
   // update barometer reference altitude
   state.baro_ref_alt = raw_sensors.baro.alt;
   // log the new home position
   RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Home Position Updated to Baro Ref Alt: " + std::to_string(state.baro_ref_alt));
   // update reference coordinates in position estimation
   localizationkf.reset(); // reinitialize the filter
   _homeSet = true;
}

void Estimator::updateOrientation(const float &gx, const float &gy, const float &gz,
                                  const float &ax, const float &ay, const float &az,
                                  const float &mx, const float &my, const float &mz, float dt)
{

   // calculate orientation solution
   madgwick.setDeltaT(dt); // update integration time
   //TODO Fix this
   //!need to convert frame from NED to NWU
   madgwick.update(gx, gy, gz, ax, ay, az, mx, my, mz);
   // madgwick.update(gx, gy, gz, ax, ay, az-2, mx, my, mz);
   // madgwick.update(gx, gy, gz, ax, ay, az, -my, mx, mz);
   
   // update orientation
   state.orientation = madgwick.getOrientation();
   state.eulerAngles = madgwick.getEulerAngles();
   Serial.println("Orientation Updated");
}

void Estimator::updateOrientation(const float &gx, const float &gy, const float &gz,
                                  const float &ax, const float &ay, const float &az, float dt)
{

   // calculate orientation solution
   madgwick.setDeltaT(dt); // update integration time
   //TODO Fix this
   //!need to convert frame from NED to NWU
   madgwick.updateIMU(gx, gy, gz, ax, ay, az);
   // update orientation
   state.orientation = madgwick.getOrientation();
   state.eulerAngles = madgwick.getEulerAngles();
}

void Estimator::updateAngularRates(const float &gx, const float &gy, const float &gz)
{
   madgwick.getInverseRotationMatrix() * Eigen::Vector3f{gx, gy, gz}; // this might be incorrect?
}

Eigen::Vector3f Estimator::getLinearAcceleration(const float &ax, const float &ay, const float &az)
{
   //TODO Fix this
   //! need to first get the trasnformation in NWU
   // Eigen::Vector3f NWU_transformed = madgwick.getRotationMatrix() * Eigen::Vector3f{ax,-ay,-az};
   //! now transform back to NED and apply linear acceleration correction
   // return (Eigen::Vector3f{NWU_transformed[0],-NWU_transformed[1],-NWU_transformed[2]}) - Eigen::Vector3f{0, 0, 1};

   //  //! need to first get the trasnformation in NWU
   // Eigen::Vector3f NWU_transformed = madgwick.getRotationMatrix() * Eigen::Vector3f{ay,ax,-az};
   // //! now transform back to NED and apply linear acceleration correction
   // return (Eigen::Vector3f{NWU_transformed[1],NWU_transformed[0],-NWU_transformed[2]}) - Eigen::Vector3f{0, 0, 1};

   return (madgwick.getRotationMatrix() * Eigen::Vector3f{ax,ay,az}) + Eigen::Vector3f{0,0,1};
};

void Estimator::changeEstimatorState(ESTIMATOR_STATE status, std::string logmessage)
{
   if (state.estimator_state != static_cast<uint8_t>(status))
   { // check if we already have logged this
      state.estimator_state = static_cast<uint8_t>(status);
      if (status != ESTIMATOR_STATE::NOMINAL)
      {
         _systemstatus.newFlag(SYSTEM_FLAG::ERROR_ESTIMATOR, logmessage);
      }
      else if (_systemstatus.flagSetOr(SYSTEM_FLAG::ERROR_ESTIMATOR))
      {
         _systemstatus.deleteFlag(SYSTEM_FLAG::ERROR_ESTIMATOR, logmessage);
      }
   }
}

void Estimator::changeBeta(float beta)
{
   madgwick.setBeta(beta);
}

void Estimator::resetOrientation()
{
   madgwick.reset();
   RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Orientation Reset");
}

void Estimator::resetLocalization()
{
   localizationkf.reset();
   RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Estimator Reset");
}


const SensorStructs::state_t &Estimator::getData()
{
   // again as with sensors, this should be updated to return the data in a threadsafe manner
   return state;
}

void Estimator::predictLocalizationKF(const float &dt)
{
   localizationkf.predict(dt);
   state.acceleration = localizationkf.getAcceleration();
   state.velocity = localizationkf.getVelocity();
   state.position = localizationkf.getPosition();
}