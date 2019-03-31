#pragma once

#include <peripherals/base.h>

#ifdef COFFEE_WINDOWS

#pragma once

#include "../sysinfo_def.h"

#include <coffee/core/plat/plat_windows.h>

#include "../../memory/cmemory.h"
#include <intrin.h>

namespace Coffee {
namespace Environment {
namespace Windows {
struct WindowsSysInfo : SysInfoDef
{
    enum CPUID_Data
    {
        BasicInfo = 0x1,

        Extended_Start = 0x80000000,

        CPUBrand_1 = 0x80000002,
        CPUBrand_2 = 0x80000003,
        CPUBrand_3 = 0x80000004,

        CacheSizes = 0x80000006,
    };

    enum CPUID_Features
    {
        FPU,
        Virtual8086,
        DebuggingExt,
        PageSizeExt,
        TimeStamp,
        WRDMSR,
        PAE,
        MachCheckExcept,
        CMPXCHG8B,
        APIC,
        Unk1,
        SYSENTEREXIT,
        MTRR,
        PTE,
        MachCheckArch,
        CondMove,
        PageAttrTable,
        PageSizeExtended,
        ProcSerial,
        CFLUSH,
        Unk2,
        DbgStore,
        ThermMonClk,
        MMX,
        FXSaveStor,
        SSE,
        SSE2,
        SelfSnoop,
        Hyperthreading,
        ThermMon,
        Unk4,
        PendBrk,
    };

    struct proc_info
    {
        struct cache
        {
            uint16 l1;
            uint16 l1_d_size;
            uint16 l1_i_size;
            uint16 l2;
            uint16 l2_d_size;
            uint16 l2_i_size;
            uint16 l3;
            uint16 l3_d_size;
            uint16 l3_i_size;
        };
        struct proc
        {
            cache  cache;
            uint32 cores;
            uint32 threads;
        };

        Vector<proc> processors;
    };

    static uint32 CountThreads(ULONG_PTR bitm);

    static proc_info GetProcInfo();

    STATICINLINE uint32 CpuCount()
    {
        proc_info info = GetProcInfo();
        return info.processors.size();
    }
    STATICINLINE uint32 CoreCount()
    {
        uint32 c = 0;

        proc_info info = GetProcInfo();

        for(proc_info::proc const& p : info.processors)
            c += p.cores;

        return c;
    }
    STATICINLINE bool MemVirtualAvailable()
    {
        return true;
    }
    STATICINLINE uint64 MemTotal()
    {
        MEMORYSTATUSEX st;
        st.dwLength = sizeof(st);

        GlobalMemoryStatusEx(&st);

        return st.ullTotalPhys;
    }
    STATICINLINE uint64 MemAvailable()
    {
        MEMORYSTATUSEX st;
        st.dwLength = sizeof(st);

        GlobalMemoryStatusEx(&st);

        return st.ullAvailPhys;
    }
    STATICINLINE uint64 SwapTotal()
    {
        MEMORYSTATUSEX st;
        st.dwLength = sizeof(st);

        GlobalMemoryStatusEx(&st);

        return st.ullTotalPageFile;
    }
    STATICINLINE uint64 SwapAvailable()
    {
        MEMORYSTATUSEX st;
        st.dwLength = sizeof(st);

        GlobalMemoryStatusEx(&st);

        return st.ullAvailPageFile;
    }

    static HWDeviceInfo Processor();

    STATICINLINE bigscalar ProcessorFrequency()
    {
        LARGE_INTEGER e;
        QueryPerformanceFrequency(&e);
        return e.QuadPart / 1000000.0;
    }
    STATICINLINE bool HasFPU()
    {
        return IsProcessorFeaturePresent(PF_FLOATING_POINT_EMULATED) == 0;
    }
    STATICINLINE bool HasPAE()
    {
        return IsProcessorFeaturePresent(PF_PAE_ENABLED);
    }
    STATICINLINE uint64 ProcessorCacheSize()
    {
        auto info = GetProcInfo();

        return info.processors.front().cache.l1_d_size;
    }

    static bool HasHyperThreading();

    STATICINLINE NetStatusFlags NetStatus()
    {
        return NetStatLocalOnly;
    }
    static CString GetSystemVersion();

    static HWDeviceInfo DeviceName();

    static HWDeviceInfo Motherboard();

    static HWDeviceInfo Chassis();
};

} // namespace Windows
} // namespace Environment

using PowerInfo = PowerInfoDef;
using SysInfo   = Environment::Windows::WindowsSysInfo;
} // namespace Coffee

#endif
