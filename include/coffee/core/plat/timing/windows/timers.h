#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#ifndef COFFEE_CORE_PLAT_TIMING_TIMERS_H
#define COFFEE_CORE_PLAT_TIMING_TIMERS_H

#include <coffee/core/plat/plat_windows.h>

namespace Coffee{
namespace CFunctional {
    struct WindowsPerformanceCounterData {
        WindowsPerformanceCounterData()
        {
            QueryPerformanceFrequency(&freq);
        }

        Mutex perf_lock;
        LARGE_INTEGER freq;
        AtomicBool loaded;
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

struct WindowsTime : PosixIshTimeDef
{
#if !defined(COFFEE_USE_CHRONOTIME)
    static uint64 Microsecond() { return CFunctional::_win_api_get_time(); }

    static Timestamp CurrentTimestamp() { return Microsecond()/1000000; }
    static uint64 CurrentMicroTimestamp() { return Microsecond(); }
#endif

	static DateTime GetDateTime(Timestamp ts)
	{
		DateTime t;

		time_t ts_t = ts;
		tm* tmp = localtime(&ts_t);

		memcpy(&t,tmp,sizeof(tm));

		return t;
	}

	static CString StringDate(cstring fmt, DateTime dt)
	{
		CString out;
		out.resize(40);

		int len = std::strftime(&out[0],out.size(),fmt,&dt);

		return out;
	}
	static CString ClockString() 
	{
		Timestamp ts = CurrentTimestamp();
		DateTime dt = GetDateTime(ts);
		return StringDate("%H:%M:%S",dt);
	}
	static CString FormattedCurrentTime(cstring fmt)
	{
		Timestamp ts = CurrentTimestamp();
		DateTime dt = GetDateTime(ts);
		return StringDate(fmt,dt);
	}
	static CString LocalTimeString(Timestamp ts)
	{
		DateTime dt = GetDateTime(ts);
		return StringDate("%c", dt);
	}
};

using Time = WindowsTime;
using CElapsedTimer = CFunctional::_plat_timer_ll_milli;
using CElapsedTimerMicro = CFunctional::_plat_timer_ll_micro;
using CElapsedTimerD = CFunctional::_plat_timer_scalar_micro;

}

#endif
#endif
