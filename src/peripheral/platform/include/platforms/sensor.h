#pragma once

#include "android/sensor.h"
#include "maemo/sensor.h"

#include "base/sensor.h"

namespace Coffee {

#if !defined(COFFEE_ANDROID) && !defined(COFFEE_MAEMO)
/* This is an optional API, so we put this here */
using Sensor = platform::sensor::Sensor_Def;
#else
using Sensor = platform::sensor::Sensor;
#endif
}
