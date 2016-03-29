#pragma once

#include "application_data.h"
#include "coffee_macros.h"

namespace Coffee{

extern ApplicationData_t CoffeeApplicationData;

const constexpr cstring CoffeeCompilerString = C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                                                  C_COMPILER_VER_MAJ,
                                                                  C_COMPILER_VER_MIN,
                                                                  C_COMPILER_VER_REV);

const constexpr cstring CoffeeArchString = COFFEE_ARCH;

const constexpr cstring CoffeeBuildString = COFFEE_BUILD_STRING;

}
