#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H

#include "../plat_primary_identify.h"
#include "../../coffee_mem_macros.h"
#include "../../base/math/cmath.h"
#include "../../types/cdef/infotypes.h"

#include "../memory/stlstring_ops.h"
#include "../memory/string_ops.h"

#include <thread>

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
     * \brief Get string representing system architecture (eg. win64, lin64, mac64, ios64, and32)
     * \return
     */
    CString GetSystemString()
    {
        CString base = C_SYSTEM_STRING;
        base.resize(3);
        base = Mem::StrUtil::lower(base);
        base += Mem::Convert::uinttostring(BitNess());
        base += "_";
        base += COFFEE_ARCH;
        return base;
    }

    STATICINLINE
    /*!
     * \brief Only for use in safe environments, such as internal servers.
     * \return
     */
    CString HostName()
    {
        return "localhost";
    }

    STATICINLINE
    /*!
     * \brief CPU count in total
     * \return
     */
    CoreCnt CpuCount()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief CPU core count in total, all CPUs
     * \return
     */
    CoreCnt CoreCount()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief CPU thread count in total, all CPUs
     * \return
     */
    ThrdCnt ThreadCount()
    {
        return std::thread::hardware_concurrency();
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

    STATICINLINE
    /*!
     * \brief Calculate a 'smart' amount of tasks to launch based upon amount of tasks
     * \param worksize Amount of tasks
     * \param weight Weight given to each tasks. Should only be modified if each task is significant in size.
     * \return An estimated value for what would be a suitable amount of tasks
     */
    ThrdCnt SmartParallelism(uint64 worksize, uint64 weight = 1)
    {
        if(worksize*weight <= ThreadCount())
        {
            return 1;
        }else if(worksize*weight <= CMath::pow<uint64>(Parallelism(),3))
        {
            return ThreadCount();
        }else
            return Parallelism();
    }

    STATICINLINE
    /*!
     * \brief Whether system supports virtual memory
     * \return
     */
    bool MemVirtualAvailable()
    {
        return true;
    }

    STATICINLINE
    /*!
     * \brief Total amount of memory in system
     * \return
     */
    MemUnit MemTotal()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Available free memory that can be used
     * \return
     */
    MemUnit MemAvailable()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Total amount of swap in system
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

    STATICINLINE
    /*!
     * \brief Info about the system's processor
     * \return
     */
    HWDeviceInfo Processor()
    {
        return HWDeviceInfo("Generic Processor","0x0");
    }

    STATICINLINE
    /*!
     * \brief Current processor frequency in GHz, allowed to be inaccurate if hardware clocks particular cores differently
     * \return
     */
    bigscalar ProcessorFrequency()
    {
        return 1.0;
    }

    STATICINLINE
    /*!
     * \brief Whether the system has an FPU, has a great impact on floating-point calculations
     * \return
     */
    bool HasFPU()
    {
        return false;
    }

    STATICINLINE
    bool HasFPUExceptions()
    {
        return false;
    }

    STATICINLINE
    /*!
     * \brief Cache size can be used to optimize tight loops, or just to make screens look tacky
     * \return
     */
    MemUnit ProcessorCacheSize()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Useful in correlation to core count
     * \return
     */
    bool HasHyperThreading()
    {
        return false;
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
};

struct PowerInfoDef
{
    STATICINLINE
    /*!
     * \brief If device is connected to wall power
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
};

}

#endif

