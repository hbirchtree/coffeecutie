#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../process_def.h"

namespace Coffee{

struct AndroidProcessPropertyApi : ProcessPropertyDef
{
    STATICINLINE void CoreDumpEnable()
    {
    }
};

using ProcessProperty = AndroidProcessPropertyApi;

}
#endif