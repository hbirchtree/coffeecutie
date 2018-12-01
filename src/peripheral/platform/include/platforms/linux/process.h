#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_LINUX)

#include <platforms/base/process.h>

#include <peripherals/libc/types.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/stl/types.h>

#include <sys/resource.h>
#include <unistd.h>

#if defined(COFFEE_NO_RUSAGE_THREAD)
#undef RUSAGE_THREAD
#define RUSAGE_THREAD RUSAGE_SELF
#endif

namespace platform {
namespace env {
namespace Linux {

using namespace ::libc_types;
using namespace ::stl_types;

struct ProcessProperty : ProcessPropertyDef
{
    using PID     = pid_t;
    using MemUnit = i64;

    static MemUnit Mem(PID);

#if !defined(COFFEE_ANDROID)
    STATICINLINE void CoreDumpEnable()
    {
        struct rlimit lim;
        lim.rlim_cur = lim.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE, &lim);
    }
    STATICINLINE bool CoreAffinity(Thread& thr, u32 i)
    {
#if !defined(COFFEE_MAEMO)
        cpu_set_t t;
        CPU_ZERO(&t);
        CPU_SET(i, &t);
        int out =
            pthread_setaffinity_np(thr.native_handle(), sizeof(cpu_set_t), &t);
        return out == 0;
#else
        return false;
#endif
    }

    STATICINLINE PID Pid()
    {
        return getpid();
    }

    STATICINLINE i32 CpuTime(PID)
    {
        return 0;
    }

    STATICINLINE i64 UserTime()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF, &rs) != 0)
            return 0;
        return rs.ru_utime.tv_sec * 1000000 + rs.ru_utime.tv_usec;
    }
    STATICINLINE i64 KernTime()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF, &rs) != 0)
            return 0;
        return rs.ru_stime.tv_sec * 1000000 + rs.ru_stime.tv_usec;
    }

    STATICINLINE i64 ThreadPageFaults()
    {
        rusage rs;
        if(getrusage(RUSAGE_THREAD, &rs) != 0)
            return 0;
        return rs.ru_majflt;
    }
    STATICINLINE i64 ThreadSoftPageFaults()
    {
        rusage rs;
        if(getrusage(RUSAGE_THREAD, &rs) != 0)
            return 0;
        return rs.ru_minflt;
    }
    STATICINLINE i64 Swaps()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF, &rs) != 0)
            return 0;
        return rs.ru_nswap;
    }
    STATICINLINE MemUnit ContextSwitches()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF, &rs) != 0)
            return 0;
        return rs.ru_nvcsw;
    }
    STATICINLINE MemUnit BadContextSwitches()
    {
        rusage rs;
        if(getrusage(RUSAGE_SELF, &rs) != 0)
            return 0;
        return rs.ru_nivcsw;
    }
#endif
};

struct MemMap
{
    struct Entry
    {
        CString        name; /*!< Path name if applicable*/
        u64            inode;
        u64            start;  /*!< Start memory address*/
        u64            end;    /*!< End memory address*/
        szptr          offset; /*!< Offset into file*/
        semantic::RSCA access;

        inline u64 size()
        {
            return end - start;
        }
    };

    struct ProcMap : Vector<Entry>
    {
    };

    static bool GetProcMap(ProcessProperty::PID pid, ProcMap& target);
};

} // namespace Linux
} // namespace env

#if !defined(COFFEE_ANDROID)
using ProcessProperty = env::Linux::ProcessProperty;
#endif

} // namespace platform

#endif
