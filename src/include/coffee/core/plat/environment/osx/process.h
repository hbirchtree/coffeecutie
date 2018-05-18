#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../process_def.h"

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacProcessProperty : ProcessPropertyDef
{
};

}
}

using ProcessProperty = Environment::Mac::MacProcessProperty;

}
#endif
