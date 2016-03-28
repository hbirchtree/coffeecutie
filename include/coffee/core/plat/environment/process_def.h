#ifndef COFFEE_CORE_PLAT_ENV_PROCESS_DEF_H
#define COFFEE_CORE_PLAT_ENV_PROCESS_DEF_H

#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace Environment{
struct ProcessPropertyDef
{
    using PID = uint64;
    /*!
     * \brief Initializes core dumping for the process, useful for information on crashes
     */
    static void CoreDumpEnable()
    {
    }
    /*!
    * \brief Idea from: http://eli.thegreenplace.net/2016/c11-threads-affinity-and-hyperthreading/
    * \param thread
    * \param core
    */
    static bool CoreAffinity(Threads::Thread&, CoreCnt)
    {
        return false;
    }

    /*!
     * \brief Current process PID
     * \return
     */
    static PID Pid()
    {
    }
    /*!
     * \brief Return process' resident memory usage, as in used memory within physical storage
     * \param pid Target PID
     * \return
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
    static int32 CpuUsage(PID)
    {
        return 0;
    }
};

}
}

#endif

#include "android/process.h"
#include "linux/process.h"
#include "windows/process.h"