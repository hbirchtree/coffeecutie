#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H

#include <coffee/core/CTypes>
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
    static uint32 CpuCount();
    /*!
     * \brief CPU core count in total, all CPUs
     * \return
     */
    static uint32 CoreCount();
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
    static bool MemVirtualAvailable();
    /*!
     * \brief Total amount of memory in system
     * \return
     */
    static int64 MemTotal();
    /*!
     * \brief Available free memory that can be used
     * \return
     */
    static int64 MemAvailable();

    /*!
     * \brief Total amount of swap in system
     * \return
     */
    static int64 SwapTotal();
    /*!
     * \brief Available free swap space in system, not very useful, system-managed
     * \return
     */
    static int64 SwapAvailable();

    /*!
     * \brief Reports networking status
     * \return
     */
    static NetStatusFlags NetStatus();
};

}

#endif

#include "linux/sysinfo.h"