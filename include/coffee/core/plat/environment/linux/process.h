#include "../../plat_primary_identify.h"

#ifdef COFFEE_LINUX
#ifndef COFFEE_CORE_PLAT_ENV_PROCESS_H
#define COFFEE_CORE_PLAT_ENV_PROCESS_H

#include "../../../coffee_strings.h"
#include "../process_def.h"

#include <sys/resource.h>
#include <unistd.h>

namespace Coffee{
namespace Environment{
namespace Linux{
struct LinuxProcessProperty : ProcessPropertyDef
{
    STATICINLINE void CoreDumpEnable()
    {
        struct rlimit lim;
        lim.rlim_cur = lim.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE,&lim);
    }
    STATICINLINE bool CoreAffinity(std::thread& thr, uint32 i)
    {
        cpu_set_t t;
        CPU_ZERO(&t);
        CPU_SET(i,&t);
        int out = pthread_setaffinity_np(thr.native_handle(),
                                         sizeof(cpu_set_t),
                                         &t);
        return out==0;
    }

    STATICINLINE PID Pid()
    {
        return getpid();
    }
    STATICINLINE MemUnit MemResident(PID pid)
    {
        rusage rs;
        if(getrusage(pid,&rs)!=0)
            return 0;
        return rs.ru_idrss+rs.ru_isrss;
    }
    STATICINLINE MemUnit MemVirtual(PID pid)
    {
        rusage rs;
        if(getrusage(pid,&rs)!=0)
            return 0;
        return rs.ru_nvcsw;
    }
    STATICINLINE int32 CpuUsage(PID)
    {
        return 0;
    }
};
}
}

using ProcessProperty = Environment::Linux::LinuxProcessProperty;

}

#endif
#endif