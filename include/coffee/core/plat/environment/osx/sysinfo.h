#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../sysinfo_def.h"

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacSysInfo : SysInfoDef
{
};

}
}

using SysInfo = Environment::Mac::MacSysInfo;

}
#endif
