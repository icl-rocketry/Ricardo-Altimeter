#include "logger.h"
#include "system.h"

void Logger::esplogData(System& system)
{
    auto line = makeLogLine(system);
    ESP_LOGI("Logger", "%s", line.c_str());
}

std::string Logger::makeLogLine(System& system)
{
    auto raw_sensors     = system.sensors.getData();
    auto estimator_state = system.estimator.getData();
    uint64_t t_ms        = millis();

    // (populate your fields exactly as you already do)
    pd = estimator_state.position(2);
    vd = estimator_state.velocity(2);
    an = estimator_state.acceleration(0); ae = estimator_state.acceleration(1); ad = estimator_state.acceleration(2);
    q0 = estimator_state.orientation.w(); q1 = estimator_state.orientation.x();
    q2 = estimator_state.orientation.y(); q3 = estimator_state.orientation.z();
    ax = raw_sensors.accelgyro.ax; ay = raw_sensors.accelgyro.ay; az = raw_sensors.accelgyro.az;
    h_ax = raw_sensors.accel.ax;   h_ay = raw_sensors.accel.ay;   h_az = raw_sensors.accel.az;
    gx = raw_sensors.accelgyro.gx; gy = raw_sensors.accelgyro.gy; gz = raw_sensors.accelgyro.gz;
    mx = raw_sensors.mag.mx;       my = raw_sensors.mag.my;       mz = raw_sensors.mag.mz;
    baro_temp  = raw_sensors.baro.temp; baro_press = raw_sensors.baro.press; baro_alt = raw_sensors.baro.alt;
    batt_voltage = raw_sensors.logicrail.volt; batt_percent = raw_sensors.logicrail.percent;

    char buf[1024];
    int n = std::snprintf(
        buf, sizeof(buf),
        "%" PRIu64 ",%.3f,%.3f,%.3f,%.3f,%.3f,"
        "%.5f,%.5f,%.5f,%.5f,"
        "%.3f,%.3f,%.3f,"
        "%.3f,%.3f,%.3f,"
        "%.3f,%.3f,%.3f,"
        "%.3f,%.3f,%.3f,"
        "%.2f,%.1f,%.2f,"
        "%u,%u\n",
        t_ms,
        pd, vd, an, ae, ad,
        q0, q1, q2, q3,
        ax, ay, az,
        h_ax, h_ay, h_az,
        gx, gy, gz,
        mx, my, mz,
        baro_temp, baro_press, baro_alt,
        (unsigned)batt_voltage, (unsigned)batt_percent);

    if (n < 0) return {};
    if (n >= (int)sizeof(buf)) n = sizeof(buf) - 1; // truncated
    return std::string(buf, (size_t)n);
}

std::string Logger::makeHeaderLine()
{
    return "t_ms,pd,vd,an,ae,ad,q0,q1,q2,q3,ax,ay,az,hax,hay,haz,gx,gy,gz,mx,my,mz,baro_temp,baro_press,baro_alt,batt_mv,batt_pct\n";
}