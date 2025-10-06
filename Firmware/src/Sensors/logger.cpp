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
    uint32_t status      = system.systemstatus.getStatus();

    // (populate your fields exactly as you already do)
    pn = estimator_state.position(0);  pe = estimator_state.position(1);  pd = estimator_state.position(2);
    vn = estimator_state.velocity(0);  ve = estimator_state.velocity(1);  vd = estimator_state.velocity(2);
    an = estimator_state.acceleration(0); ae = estimator_state.acceleration(1); ad = estimator_state.acceleration(2);
    roll = estimator_state.eulerAngles(0); pitch = estimator_state.eulerAngles(1); yaw = estimator_state.eulerAngles(2);
    q0 = estimator_state.orientation.w(); q1 = estimator_state.orientation.x();
    q2 = estimator_state.orientation.y(); q3 = estimator_state.orientation.z();
    ax = raw_sensors.accelgyro.ax; ay = raw_sensors.accelgyro.ay; az = raw_sensors.accelgyro.az;
    h_ax = raw_sensors.accel.ax;   h_ay = raw_sensors.accel.ay;   h_az = raw_sensors.accel.az;
    gx = raw_sensors.accelgyro.gx; gy = raw_sensors.accelgyro.gy; gz = raw_sensors.accelgyro.gz;
    mx = raw_sensors.mag.mx;       my = raw_sensors.mag.my;       mz = raw_sensors.mag.mz;
    baro_temp  = raw_sensors.baro.temp; baro_press = raw_sensors.baro.press; baro_alt = raw_sensors.baro.alt;
    batt_voltage = raw_sensors.logicrail.volt; batt_percent = raw_sensors.logicrail.percent;
    system_status = status;

    char buf[1024];
    int n = std::snprintf(buf, sizeof(buf),
        "t_ms=%" PRIu64 " status=0x%08" PRIx32
        " pn=%.3f pe=%.3f pd=%.3f"
        " vn=%.3f ve=%.3f vd=%.3f"
        " an=%.3f ae=%.3f ad=%.3f"
        " roll=%.2f pitch=%.2f yaw=%.2f"
        " q0=%.5f q1=%.5f q2=%.5f q3=%.5f"
        " ax=%.3f ay=%.3f az=%.3f"
        " hax=%.3f hay=%.3f haz=%.3f"
        " gx=%.3f gy=%.3f gz=%.3f"
        " mx=%.3f my=%.3f mz=%.3f"
        " baro_temp=%.2f baro_press=%.1f baro_alt=%.2f"
        " batt_mv=%u batt_pct=%u",
        t_ms, status,
        pn, pe, pd, vn, ve, vd, an, ae, ad,
        roll, pitch, yaw, q0, q1, q2, q3,
        ax, ay, az, h_ax, h_ay, h_az, gx, gy, gz,
        mx, my, mz, baro_temp, baro_press, baro_alt,
        (unsigned)batt_voltage, (unsigned)batt_percent);

    if (n < 0) return {};
    if (n >= (int)sizeof(buf)) n = sizeof(buf) - 1;  // truncated
    return std::string(buf, (size_t)n);
}