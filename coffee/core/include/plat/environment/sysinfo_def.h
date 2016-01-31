#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H

#include <coffee/core/CTypes>
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

    /*!
     * \brief CPU count in total
     * \return
     */
    static uint32 CpuCount();
    /*!
     * \brief CPU core count in total, all CPUs
     * \return
     */
    static uint32 CpuCoreCount();
    /*!
     * \brief CPU thread count in total, all CPUs
     * \return
     */
    static uint32 CpuThreadCount()
    {
        return std::thread::hardware_concurrency();
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