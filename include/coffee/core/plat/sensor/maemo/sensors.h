#pragma once

#include "../sensor_def.h"

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
