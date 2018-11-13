#pragma once

#include <peripherals/libc/types.h>

namespace Coffee{

/*!
 * \brief Prototype for Coffee-managed programs, does platform abstraction
 */
using CoffeeMainWithArgs = int(*)(int,char**);

}
