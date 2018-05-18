#pragma once

#include <coffee/core/types/vector_types.h>

namespace Coffee{
namespace Sensor{

struct SensorAPI_Def
{
    /* Direction of gravity */
    STATICINLINE Vecf3 Gravity(){return {};}
    /* Acceleration on device, does not include gravity */
    STATICINLINE Vecf3 Acceleration(){return {};}

    /* Absolute orientation of device */
    STATICINLINE Vecf3 Orientation(){return {};}
    /* rad/s on device */
    STATICINLINE Vecf3 Gyroscope(){return {};}

    /* Describes geomagnetic field around device, also called a compass */
    STATICINLINE Vecf3 Magnetometer(){return {};}

    /* Light measured by ambient light sensor */
    STATICINLINE scalar Lux(){return 0;}

    STATICINLINE uint32 StepCounter(){return 0;}

    STATICINLINE scalar Proximity(){return 0;}
};

}
}
