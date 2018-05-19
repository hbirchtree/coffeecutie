#pragma once

#include "../plat_primary_identify.h"
#include "../../coffee_mem_macros.h"
#include "../../base/math/cmath.h"
#include "../../types/cdef/infotypes.h"

#include "../memory/stlstring_ops.h"
#include "../memory/string_ops.h"
#include "../../types/tdef/stlfunctypes.h"

namespace Coffee{

struct SysInfoDef
{
    enum NetStatusFlags
    {
        NetStatDisconnected = 0x0,
        NetStatConnected = 0x1,

        NetStatLocalOnly = 0x2,
    };

    STATICINLINE
    /*!
     * \brief Whether this build is 32-bit, 64-bit, 128-bit, blah.
     * \return
     */
    uint32 BitNess()
    {
        return C_SYSTEM_BITNESS;
    }

    STATICINLINE
    /*!
     * \brief Should return operating system version or similar.
     * \return
     */
    CString GetSystemVersion()
    {
        return "?";
    }

    static
    /*!
     * \brief Return a primary operating system identifier, no version
     * \return
     */
    CString GetSystemString();

    static
    /*!
     * \brief Only for use in safe environments, such as internal servers.
     * \return
     */
    CString HostName();

    STATICINLINE
    /*!
     * \brief Return amount of processor packages.
     * For NUMA systems, this is the amount of NUMA nodes.
     * ARM big.LITTLE still count as single-package systems.
     * \return
     */
    CoreCnt CpuCount()
    {
        return 1;
    }

    STATICINLINE
    /*!
     * \brief Return core count of processor in use.
     *  In NUMA systems, this shall be core count of the single processors.
     * \param i Processor to examine, max count is CpuCount()
     * \return
     */
    CoreCnt CoreCount(u32 i = 0)
    {
        C_UNUSED(i);
        return 0;
    }

    STATICINLINE
    /*!
     * \brief CPU thread count in total, all CPUs
     * \return
     */
    ThrdCnt ThreadCount()
    {
        ThrdCnt c = Thread::hardware_concurrency();
        return c;
    }

    STATICINLINE
    /*!
     * \brief Get amount of parallel tasks that can be launched
     * \return
     */
    ThrdCnt Parallelism()
    {
        return ThreadCount()*64;
    }

    static
    /*!
     * \brief Calculate a 'smart' amount of tasks to launch based
     *  upon amount of tasks
     * \param worksize Amount of tasks
     * \param weight Weight given to each tasks. Should only be
     *  modified if each task is significant in size.
     * \return An estimated value for what would be a suitable
     *  amount of tasks
     */
    ThrdCnt SmartParallelism(uint64 worksize, uint64 weight = 1);

    STATICINLINE
    /*!
     * \brief Whether the system supports virtual memory.
     * Only significant on weird Windows systems or
     *  low-end Linux systems.
     * \return
     */
    bool MemVirtualAvailable()
    {
        return true;
    }

    STATICINLINE
    /*!
     * \brief Total amount of memory in system. For NUMA system,
     *  this should include all nodes.
     * \return
     */
    MemUnit MemTotal()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Total amount of *available* memory in the system,
     *  counting all NUMA nodes if applicable.
     * \return
     */
    MemUnit MemAvailable()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Total amount of swap space in system. On Windows,
     *  this is the collected size of all page files.
     * On Linux, this is the collected size of all swap partitions
     *  and swap files.
     * \return
     */
    MemUnit SwapTotal()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Available free swap space in system, not very useful, system-managed
     * \return
     */
    MemUnit SwapAvailable()
    {
        return 0;
    }

    static
    /*!
     * \brief The CPUID of the processor.
     * On x86, Intel should return GenuineIntel, AMD
     * should return AuthenticAMD.
     * On virtual machines, as long as it is returned from CPUID,
     *  anything is acceptable.
     * On ARM Linux systems the definition
     *  is more coarse, as Linaro+ kernels
     *  do not provide full information.
     * \param i Processor to examine, max count is CpuCount()
     * \return
     */
    HWDeviceInfo Processor(u32 i = 0);

    STATICINLINE
    /*!
     * \brief Fill vector with processor frequencies per core.
     *  On NUMA systems, the amount of elements *should*
     *  (but does not need to) equal the amount of cores.
     * ARM big.LITTLE systems should state frequencies
     *  of both processor variants at the least.
     * \param i Processor to examine
     * \return
     */
    Vector<bigscalar> ProcessorFrequencies(u32 i = 0)
    {
        C_UNUSED(i);
        return {};
    }

    STATICINLINE
    /*!
     * \brief Return the greatest processor frequency on the system.
     * This is a legacy feature from before considering NUMA
     *  and other heterogenous systems..
     * \param i Processor to examine, max count is CpuCount()
     * \return
     */
    bigscalar ProcessorFrequency(u32 i = 0)
    {
        C_UNUSED(i);
        return 1.0;
    }

    STATICINLINE
    /*!
     * \brief Whether the system has a hardware FPU,
     *  and does not process floating-point in pure software.
     * On ARM, it is enough to check for VFPv* or NEON.
     * On x86, the "fpu" CPUID flag is enough.
     * \return
     */
    bool HasFPU()
    {
        return false;
    }

    STATICINLINE
    /*!
     * \brief Whether FPU errors trigger exceptions/signals.
     * Is not very much used.
     * \return
     */
    bool HasFPUExceptions()
    {
        return false;
    }

    STATICINLINE
    /*!
     * \brief Loose definition, but if possible should return L1 cache size.
     * This API is poorly implemented.
     * \param i Processor to examine, max count is CpuCount()
     * \return
     */
    MemUnit ProcessorCacheSize(u32 i = 0)
    {
        C_UNUSED(i);
        return 0;
    }

    STATICINLINE

    /*!
     * \brief Often provided as CoreCount() != ThreadCount(),
     * should give a heuristic of whether the system maps
     *  multiple execution threads to single physical cores.
     * \param i Processor to examine, max count is CpuCount()
     * \return
     */
    bool HasHyperThreading()
    {
        return false;
    }

    STATICINLINE
    /*!
     * \brief Whether the 32-bit system supports PAE, doesn't really apply to 64-bit
     * \return
     */
    bool HasPAE()
    {
        return true;
    }

    STATICINLINE
    /*!
     * \brief Reports networking status
     * \return
     */
    NetStatusFlags NetStatus()
    {
        return NetStatLocalOnly;
    }

    /*!
     * \brief If available through DMI or otherwise,
     *  should return information on what retail device it might be.
     *  Most custom-built desktop computers will not apply,
     *  but laptops and phones will.
     * \return
     */
    static
    HWDeviceInfo DeviceName();

    /*!
     * \brief If possible, return the motherboard information from
     *  DMI or otherwise.
     * On Android, this is the Android SoC designation,
     * eg. flo for Nexus 7 (2013).
     * \return
     */
    static
    HWDeviceInfo Motherboard();

    /*!
     * \brief Mostly for laptops and phones, return a description
     *  of the chassis vendor.
     * \return
     */
    static
    HWDeviceInfo Chassis();

    /*!
     * \brief Provide a BIOS version for the motherboard.
     *  Mostly applicable to x86 motherboards, but could return
     *  bootloader or firmware version for phone.
     * \return
     */
    static
    HWDeviceInfo BIOS();
};


struct PowerInfoDef
{
    struct Temp
    {
        scalar current;
        scalar trip_point;
    };

    STATICINLINE
    /*!
     * \brief If device is connected to wall power, often same as charging status,
     *   but primarily refers to having little limitation to power usage.
     * \return
     */
    bool IsPowered()
    {
        return true;
    }
    STATICINLINE
    /*!
     * \brief If device is charging
     * \return
     */
    bool IsCharging()
    {
        return false;
    }
    STATICINLINE
    /*!
     * \brief If device has battery at all
     * \return
     */
    bool HasBattery()
    {
        return false;
    }
    STATICINLINE
    /*!
     * \brief Battery percentage
     * \return
     */
    uint16 BatteryPercentage()
    {
        return 0;
    }
    STATICINLINE
    /*!
     * \brief Remaining minutes of battery
     * \return
     */
    uint32 BatteryLifetime()
    {
        return 0;
    }
    STATICINLINE
    /*!
     * \brief Basic system temperature in Celsius
     * \return
     */
    Temp CpuTemperature()
    {
        return {0.f,0.f};
    }
};

struct DeviceInfoDef
{
    static CString Manufacturer(){return {};}
    static CString Model(){return {};}
    static CString Board(){return {};}

    struct Screen
    {
        enum TouchType
        {
            None,
            Resistive,
            Capacitive,
        };

        scalar size;
        u32 density;
        u32 res_x;
        u32 res_y;

        u32 color_space;

        TouchType touch_type;
    };

    static Screen Screen(){return {};}

    static scalar Weight(){return {};}
};

#if defined(COFFEE_STUBBED_SYSINFO)
using SysInfo = SysInfoDef;
using PowerInfo = PowerInfoDef;
#endif

}