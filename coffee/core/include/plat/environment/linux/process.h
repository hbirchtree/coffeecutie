#include <plat/unix/unix_core.h>

#ifdef COFFEE_LINUX
#ifndef COFFEE_CORE_PLAT_ENV_PROCESS_H
#define COFFEE_CORE_PLAT_ENV_PROCESS_H

#include "../process_def.h"
#include <coffee/core/CTypes>
#include <sys/resource.h>
#include <unistd.h>
#include <coffee_strings.h>

namespace Coffee{

struct ProcessProperty : ProcessPropertyDef
{
    STATICINLINE void CoreDumpEnable()
    {
        struct rlimit lim;
        lim.rlim_cur = lim.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE,&lim);
    }
    STATICINLINE void CoreAffinity(std::thread& thr, uint32 i)
    {
        cpu_set_t t;
        CPU_ZERO(&t);
        CPU_SET(i,&t);
        int out = pthread_setaffinity_np(thr.native_handle(),
                                         sizeof(cpu_set_t),
                                         &t);
        if(out!=0)
        {
            cLog(__FILE__,__LINE__,
                 CFStrings::Plat_Unix_Core_Lib,
                 CFStrings::Plat_Cpu_Affinity_Error);
        }
    }

    STATICINLINE int64 Pid()
    {
        return getpid();
    }
    STATICINLINE int64 MemResident(int64 pid)
    {
        rusage rs;
        if(getrusage(pid,&rs)!=0)
            return 0;
        return rs.ru_idrss+rs.ru_isrss;
    }
    STATICINLINE int64 MemVirtual(int64 pid)
    {
        rusage rs;
        if(getrusage(pid,&rs)!=0)
            return 0;
        return rs.ru_nvcsw;
    }
    STATICINLINE int32 CpuUsage(int64)
    {
        return 0;
    }
};

}

#endif
#endif