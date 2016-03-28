#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../process_def.h"

namespace Coffee{
namespace Environment{
namespace Android{
struct AndroidProcessPropertyApi : ProcessPropertyDef
{
};
}
}

using ProcessProperty = Environment::Android::AndroidProcessPropertyApi;

}
#endif