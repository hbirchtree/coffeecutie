#pragma once

#ifdef __cplusplus

#include "application_data.h"
#include "coffee_macros.h"

namespace Coffee{

using CoffeeApplicationData = ApplicationData_t;

}


extern "C" {
#endif
extern const char* CoffeeLicenseString[];
extern unsigned int CoffeeLicenseCount;
#ifdef __cplusplus
}
#endif
