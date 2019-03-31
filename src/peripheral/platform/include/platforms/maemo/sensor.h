#pragma once

#include <platforms/base/sensor.h>

#if defined(COFFEE_MAEMO)
namespace platform {
namespace sensor {
namespace maemo {

struct Sensor : Sensor_Def
{
    static Vecf3  Gravity();
    static scalar Lux();
};

} // namespace maemo

using Sensors = maemo::Sensor;

} // namespace sensor
} // namespace platform
#endif
