#pragma once

#include "integertypes.h"

namespace Coffee{

/*!
 * \brief Prototype for Coffee-managed programs, does platform abstraction
 */
using CoffeeMainWithArgs = int(*)(int,char**);

}