#pragma once

#ifdef COFFEE_ANDROID

#include "../sysinfo_def.h"

namespace Coffee{

struct AndroidSysInfo : SysInfoDef
{
};

using SysInfo = AndroidSysInfo;

}

#endif