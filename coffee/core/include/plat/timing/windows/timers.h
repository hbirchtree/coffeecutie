#include <plat/platform_detect.h>

#ifdef COFFEE_WINDOWS
#ifndef COFFEE_CORE_PLAT_TIMING_TIMERS_H
#define COFFEE_CORE_PLAT_TIMING_TIMERS_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Coffee{
namespace CFunctional {
    namespace CDebugHelpers {
        inline static cstring_w* coffee_callstack(szptr* len, uint32 stackreduce)
        {
            C_STUBBED("Callstack dumping");
            *len = 0;
            return nullptr;
        }
    }

    struct WindowsPerformanceCounterData {
        WindowsPerformanceCounterData()
        {
            QueryPerformanceFrequency(&freq);
        }

        std::mutex perf_lock;
        LARGE_INTEGER freq;
        std::atomic_bool loaded;
    };

    static WindowsPerformanceCounterData _win_perfcounter_data;

    inline C_FORCE_INLINE uint64 _win_api_get_time()
    {
        LARGE_INTEGER tmp;
        QueryPerformanceCounter(&tmp);
        uint64 tick = tmp.QuadPart;
        return (tick*1000000)/_win_perfcounter_data.freq.QuadPart;
    }

    template<typename T>
    class _cbasic_windows_timer : public _cbasic_timer<T>
    {
    public:
        virtual ~_cbasic_windows_timer() {}
        void start()
        {
            _start = _curr_time();
        }
        T elapsed()
        {
            return _curr_time() - _start;
        }
    protected:
        virtual T _curr_time() = 0;
    private:
        T _start = 0;
    };

    class _plat_timer_ll_milli : public _cbasic_windows_timer<uint64>
    {
    protected:
        uint64 _curr_time()
        {
            return _win_api_get_time()/1000;
        }
    };
    class _plat_timer_ll_micro : public _cbasic_windows_timer<uint64>
    {
    protected:
        uint64 _curr_time()
        {
            return _win_api_get_time();
        }
    };
    class _plat_timer_scalar_micro : public _cbasic_windows_timer<bigscalar>
    {
    protected:
        bigscalar _curr_time()
        {
            return ((bigscalar)_win_api_get_time())/1000000.0;
        }
    };
}

}

#endif
#endif