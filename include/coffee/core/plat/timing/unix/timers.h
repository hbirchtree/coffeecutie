#include "../../platform_detect.h"

#if defined(COFFEE_UNIXPLAT) || defined(COFFEE_ANDROID)

#ifndef COFFEE_CORE_PLAT_TIMING_TIMERS_H
#define COFFEE_CORE_PLAT_TIMING_TIMERS_H

#include "../timing_def.h"
#include <sys/time.h>
#include <ctime>
#include <memory.h>

namespace Coffee{
namespace CFunctional{

template<typename T>
class _cbasic_posix_timer : public _cbasic_timer<T>
{
public:
    virtual ~_cbasic_posix_timer(){}
    void start(){
        _start = _curr_time();
    }
    T elapsed(){
        return _curr_time()-_start;
    }
protected:
    timeval _tv(){
        struct timeval tv;
        gettimeofday(&tv,0);
        return tv;
    }
    virtual T _curr_time() = 0;
private:
    T _start = 0;
};

class _plat_timer_ll_milli : public _cbasic_posix_timer<uint64>
{
private:
    uint64 _curr_time(){
        timeval tv = _tv();
        return tv.tv_sec*1000+tv.tv_usec/1000;
    }
};
class _plat_timer_ll_micro : public _cbasic_posix_timer<uint64>
{
private:
    uint64 _curr_time(){
        timeval tv = _tv();
        return tv.tv_sec*1000000+tv.tv_usec;
    }
};
class _plat_timer_scalar_micro : public _cbasic_posix_timer<bigscalar>
{
private:
    bigscalar _curr_time(){
        timeval tv = _tv();
        return (bigscalar)tv.tv_sec+(bigscalar)tv.tv_usec/1000000.0;
    }
};

}

struct UnixTime : TimeDef
{
    STATICINLINE uint64 Microsecond()
    {
        struct timeval t;
        gettimeofday(&t,0);
        return t.tv_usec;
    }
    STATICINLINE Timestamp CurrentTimestamp()
    {
        return std::time(NULL);
    }
    STATICINLINE uint64 CurrentMicroTimestamp()
    {
        return CurrentTimestamp()*1000000+Microsecond();
    }
    STATICINLINE DateTime GetDateTime(Timestamp ts)
    {
        DateTime t;

        time_t ts_t = ts;
        tm* tmp = localtime(&ts_t);

        memcpy(&t,tmp,sizeof(tm));

        return t;
    }

    STATICINLINE CString StringDate(cstring fmt,DateTime t)
    {
        CString out;
        out.resize(40);

        int len = std::strftime(&out[0],out.size(),fmt,&t);

        return out;
    }

    STATICINLINE CString ClockString()
    {
        Timestamp ts = CurrentTimestamp();
        DateTime dt = GetDateTime(ts);
        return StringDate("%H:%M:%S",dt);
    }

    STATICINLINE CString FormattedCurrentTime(cstring fmt)
    {
        Timestamp ts = CurrentTimestamp();
        DateTime dt = GetDateTime(ts);
        return StringDate(fmt,dt);
    }

    STATICINLINE CString LocalTimeString(Timestamp ts)
    {
        DateTime dt = GetDateTime(ts);
        return StringDate("%c",dt);
    }
};

using Time = UnixTime;
using CElapsedTimer = CFunctional::_plat_timer_ll_milli;
using CElapsedTimerMicro = CFunctional::_plat_timer_ll_micro;
using CElapsedTimerD = CFunctional::_plat_timer_scalar_micro;

}

#endif
#endif
