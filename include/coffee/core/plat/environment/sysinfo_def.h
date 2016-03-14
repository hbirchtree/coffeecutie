#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H

#include "../../coffee_mem_macros.h"
#include "../../base/math/cmath.h"
#include "../../types/cdef/infotypes.h"

#include <thread>
#include <future>
#include <iostream>

namespace Coffee{

struct SysInfoDef
{
    enum NetStatusFlags
    {
        NetStatDisconnected = 0x0,
        NetStatConnected = 0x1,

        NetStatLocalOnly = 0x2,
    };

    /*!
     * \brief CPU count in total
     * \return
     */
    STATICINLINE uint32 CpuCount()
    {
        return 0;
    }

    /*!
     * \brief CPU core count in total, all CPUs
     * \return
     */
    STATICINLINE uint32 CoreCount()
    {
        return 0;
    }

    /*!
     * \brief CPU thread count in total, all CPUs
     * \return
     */
    STATICINLINE uint64 ThreadCount()
    {
        return std::thread::hardware_concurrency();
    }

    /*!
     * \brief Get amount of parallel tasks that can be launched
     * \return
     */
    STATICINLINE uint64 Parallelism()
    {
        return ThreadCount()*64;
    }

    /*!
     * \brief Calculate a 'smart' amount of tasks to launch based upon amount of tasks
     * \param worksize Amount of tasks
     * \param weight Weight given to each tasks. Should only be modified if each task is significant in size.
     * \return An estimated value for what would be a suitable amount of tasks
     */
    STATICINLINE uint64 SmartParallelism(uint64 worksize, uint64 weight = 1)
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

    /*!
     * \brief Whether system supports virtual memory
     * \return
     */
    STATICINLINE bool MemVirtualAvailable()
    {
        return true;
    }

    /*!
     * \brief Total amount of memory in system
     * \return
     */
    STATICINLINE int64 MemTotal()
    {
        return 0;
    }

    /*!
     * \brief Available free memory that can be used
     * \return
     */
    STATICINLINE int64 MemAvailable()
    {
        return 0;
    }

    /*!
     * \brief Total amount of swap in system
     * \return
     */
    STATICINLINE int64 SwapTotal()
    {
        return 0;
    }

    /*!
     * \brief Available free swap space in system, not very useful, system-managed
     * \return
     */
    STATICINLINE int64 SwapAvailable()
    {
        return 0;
    }

    /*!
     * \brief Info about the system's processor
     * \return
     */
    STATICINLINE HWDeviceInfo Processor()
    {
        return HWDeviceInfo("Generic Processor","0x0");
    }

    /*!
     * \brief Current processor frequency in GHz, allowed to be inaccurate if hardware clocks particular cores differently
     * \return
     */
    STATICINLINE bigscalar ProcessorFrequency()
    {
        return 1.0;
    }

    /*!
     * \brief Whether the system has an FPU, has a great impact on floating-point calculations
     * \return
     */
    STATICINLINE bool HasFPU()
    {
        return false;
    }

    /*!
     * \brief Cache size can be used to optimize tight loops, or just to make screens look tacky
     * \return
     */
    STATICINLINE int64 ProcessorCacheSize()
    {
        return 0;
    }

    /*!
     * \brief Useful in correlation to core count
     * \return
     */
    STATICINLINE bool HasHyperThreading()
    {
        return false;
    }

    /*!
     * \brief Reports networking status
     * \return
     */
    STATICINLINE NetStatusFlags NetStatus()
    {
        return NetStatLocalOnly;
    }
};

}

#endif

#include "android/sysinfo.h"
#include "linux/sysinfo.h"
#include "windows/sysinfo.h"
