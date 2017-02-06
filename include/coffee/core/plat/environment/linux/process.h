#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_LINUX

#include "../../../coffee_strings.h"
#include "../process_def.h"

#include <sys/resource.h>
#include <unistd.h>

namespace Coffee{
namespace Environment{
namespace Linux{
struct LinuxProcessProperty : ProcessPropertyDef
{
    using PID = pid_t;
    using MemUnit = long;

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

    static MemUnit Mem(PID);

    STATICINLINE int32 CpuTime(PID)
    {
        return 0;
    }

    STATICINLINE int64 UserTime()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF,&rs)!=0)
            return 0;
        return rs.ru_utime.tv_sec * 1000000 + rs.ru_utime.tv_usec;
    }
    STATICINLINE int64 KernTime()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF,&rs)!=0)
            return 0;
        return rs.ru_stime.tv_sec * 1000000 + rs.ru_stime.tv_usec;
    }

    STATICINLINE int64 ThreadPageFaults()
    {
        rusage rs;
        if(getrusage(RUSAGE_THREAD,&rs)!=0)
            return 0;
        return rs.ru_majflt;
    }
    STATICINLINE int64 ThreadSoftPageFaults()
    {
        rusage rs;
        if(getrusage(RUSAGE_THREAD,&rs)!=0)
            return 0;
        return rs.ru_minflt;
    }
    STATICINLINE int64 Swaps()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF,&rs)!=0)
            return 0;
        return rs.ru_nswap;
    }
    STATICINLINE MemUnit ContextSwitches()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF,&rs)!=0)
            return 0;
        return rs.ru_nvcsw;
    }
    STATICINLINE MemUnit BadContextSwitches()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF,&rs)!=0)
            return 0;
        return rs.ru_nivcsw;
    }
};
}
}

using ProcessProperty = Environment::Linux::LinuxProcessProperty;

}

#endif
