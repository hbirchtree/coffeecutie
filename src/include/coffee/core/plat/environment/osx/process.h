#pragma once

#include <coffee/core/base.h>

#if defined(COFFEE_APPLE)

#include "../process_def.h"

namespace Coffee {
namespace Environment {
namespace Mac {

struct MacProcessProperty : ProcessPropertyDef
{
};

} // namespace Mac
} // namespace Environment

using ProcessProperty = Environment::Mac::MacProcessProperty;

} // namespace Coffee
#endif
