#pragma once

#include "integertypes.h"

namespace Coffee{

/*!
 * \brief Prototype for Coffee-managed programs, does platform abstraction
 */
using CoffeeMainWithArgs = int32(*)(int32,cstring_w*);

}