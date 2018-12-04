#pragma once

#include <platforms/base/sensor.h>

#if defined(COFFEE_ANDROID)

namespace platform {
namespace sensor {
namespace android {

struct Sensor : Sensor_Def
{
  private:
    struct AndroidSensorData;

  public:
    static Vecf3 Gravity();
    static Vecf3 Acceleration();

    static Vecf3 Orientation();
    static Vecf3 Gyroscope();

    static Vecf3 Magnetometer();

    static scalar Lux();
};

} // namespace android

using Sensors = android::Sensor;

} // namespace sensor
} // namespace platform
#endif
