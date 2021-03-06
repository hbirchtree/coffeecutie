#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/thread_types.h>

namespace platform {
namespace env {

using namespace ::libc_types;
using namespace ::stl_types;

struct ProcessPropertyDef
{
    /* Memory in this context is returned in kB */

    using PID = u64;
    /*!
     * \brief Initializes core dumping for the process, useful for information
     * on crashes
     */
    static void CoreDumpEnable()
    {
    }
    /*!
     * \brief Idea from:
     * http://eli.thegreenplace.net/2016/c11-threads-affinity-and-hyperthreading/
     * \param thread
     * \param core
     */
    static bool CoreAffinity(Thread&, CoreCnt)
    {
        return false;
    }

    /*!
     * \brief Current process PID
     * \return
     */
    static PID Pid()
    {
        return 0;
    }
    /*!
     * \brief Return process' resident memory usage, as in used memory within
     * physical storage \param pid Target PID \return
     */
    static MemUnit MemResident(PID)
    {
        return 0;
    }
    /*!
     * \brief Return process' virtual memory usage
     * \param pid Target PID
     * \return
     */
    static MemUnit MemVirtual(PID)
    {
        return 0;
    }
    /*!
     * \brief Return process' CPU usage in %, where 100 is 100% of 1 core
     * \param pid Target PID
     * \return
     */
    static i32 CpuUsage(PID)
    {
        return 0;
    }

    /*!
     * \brief Memory consumed by process
     * \return
     */
    static MemUnit Mem(PID)
    {
        return 0;
    }
    static i32 CpuTime(PID)
    {
        return 0;
    }
    static i64 UserTime()
    {
        return 0;
    }
    static i64 KernTime()
    {
        return 0;
    }
    static i64 ThreadPageFaults()
    {
        return 0;
    }
    static i64 ThreadSoftPageFaults()
    {
        return 0;
    }
    static i64 Swaps()
    {
        return 0;
    }
    static MemUnit ContextSwitches()
    {
        return 0;
    }
    static MemUnit BadContextSwitches()
    {
        return 0;
    }
};

} // namespace env
} // namespace platform
