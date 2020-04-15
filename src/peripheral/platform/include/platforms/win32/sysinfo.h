#pragma once

#include <peripherals/base.h>

#ifdef COFFEE_WINDOWS

#include <platforms/base/sysinfo.h>
#include <peripherals/error/windows.h>

#include <intrin.h>

namespace platform {
namespace env {
namespace win32 {

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
    static uint64 MemTotal();
    static uint64 MemAvailable();
    static uint64       SwapTotal();
    static uint64 SwapAvailable();

    static platform::info::HardwareDevice Processor();

    static bigscalar    ProcessorFrequency();
    static bool         HasFPU();
    static bool         HasPAE();
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

    static platform::info::HardwareDevice DeviceName();

    static platform::info::HardwareDevice Motherboard();

    static platform::info::HardwareDevice Chassis();
};

} // namespace Windows
} // namespace Environment

using PowerInfo = env::PowerInfoDef;
using SysInfo   = env::win32::WindowsSysInfo;
} // namespace Coffee

#endif
