#pragma once

#include "../sensor_def.h"

namespace Coffee{
namespace Sensor{
namespace Android{

struct Android_SensorAPI : SensorAPI_Def
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

}

using Sensors = Android::Android_SensorAPI;

}
}
