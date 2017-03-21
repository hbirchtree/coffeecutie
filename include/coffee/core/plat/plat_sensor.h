#include "sensor/sensor_def.h"
#include "sensor/android/sensor.h"
#include "sensor/maemo/sensors.h"

namespace Coffee{
namespace Sensor{

#if !defined(COFFEE_ANDROID) && !defined(COFFEE_MAEMO)
/* This is an optional API, so we put this here */
using Sensors = SensorAPI_Def;
#endif

}
}
