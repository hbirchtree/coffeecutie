#ifndef COFFEE_CORE_PLAT_ENV_PROCESS_DEF_H
#define COFFEE_CORE_PLAT_ENV_PROCESS_DEF_H

#include <coffee/core/CTypes>

namespace Coffee{

struct ProcessPropertyDef
{
    /*!
     * \brief Initializes core dumping for the process, useful for information on crashes
     */
    static void CoreDumpEnable();
    /*!
    * \brief Idea from: http://eli.thegreenplace.net/2016/c11-threads-affinity-and-hyperthreading/
    * \param thread
    * \param core
    */
    static bool CoreAffinity(std::thread&, uint32);

    /*!
     * \brief Current process PID
     * \return
     */
    static int64 Pid();
    /*!
     * \brief Return process' resident memory usage, as in used memory within physical storage
     * \param pid Target PID
     * \return
     */
    static int64 MemResident(int64 pid);
    /*!
     * \brief Return process' virtual memory usage
     * \param pid Target PID
     * \return
     */
    static int64 MemVirtual(int64 pid);
    /*!
     * \brief Return process' CPU usage in %, where 100 is 100% of 1 core
     * \param pid Target PID
     * \return
     */
    static int32 CpuUsage(int64 pid);
};

}

#endif

#include "android/process.h"
#include "linux/process.h"
#include "windows/process.h"