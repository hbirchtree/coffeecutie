#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#pragma once

#include "../process_def.h"

namespace Coffee{

namespace Windows{

struct WinProcessProperty
{
    static void CoreDumpEnable()
    {
    }
    static bool CoreAffinity(std::thread&, uint32)
    {
        return false;
    }
    static int64 Pid()
    {
        return 0;
    }
    static int64 MemResident(int64 pid)
    {
        return 0;
    }
    static int64 MemVirtual(int64 pid)
    {
        return 0;
    }
    static int32 CpuUsage(int64 pid)
    {
        return 0;
    }
};

}

using ProcessProperty = Windows::WinProcessProperty;

}

#endif