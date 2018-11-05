#pragma once

#include <coffee/core/plat/platform_detect.h>
#include "../sensor_def.h"

#if defined(COFFEE_MAEMO)
namespace Coffee{
namespace Sensor{
namespace Maemo{

struct Maemo_SensorAPI : SensorAPI_Def
{
    static Vecf3 Gravity();
    static scalar Lux();
};

}

using Sensors = Maemo::Maemo_SensorAPI;

}
}
#endif
