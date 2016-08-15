#pragma once

#include "application_data.h"
#include "coffee_macros.h"

namespace Coffee{

using CoffeeApplicationData = ApplicationData_t;

extern cstring CoffeeCompilerString;
extern cstring CoffeeArchString;
extern cstring CoffeeBuildString;
extern cstring CoffeePlatformString;

extern CString CoffeeDefaultWindowName;

}

extern "C" const char* CoffeeLicenseString[];
extern "C" unsigned int CoffeeLicenseCount;
