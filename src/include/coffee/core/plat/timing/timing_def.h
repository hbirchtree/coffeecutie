#ifndef COFFEE_CORE_PLAT_TIMING_DEF_H
#define COFFEE_CORE_PLAT_TIMING_DEF_H

#include "../../types/cdef/timetypes.h"
#include "../../types/tdef/stltypes.h"

#include "../plat_primary_identify.h"

#if defined(COFFEE_USE_CHRONOTIME)
#include <chrono>
#endif

#include <ctime>
#include <time.h>

#if defined(COFFEE_WINDOWS) || defined(COFFEE_NACL)
#include <iomanip>
#include <sstream>
#endif

namespace Coffee {
namespace Chronology {

using namespace std::chrono;

template<typename cast_to, typename T>
FORCEDINLINE u64 time_to_uint64(T d)
{
    return C_FCAST<u64>(duration_cast<cast_to>(d).count());
}

struct TimeDef
{
/*!
 * \brief Current microsecond
 * \return
 */
#if defined(COFFEE_USE_CHRONOTIME)
    static u64 Microsecond()
    {
        return time_to_uint64<microseconds>(
            high_resolution_clock::now().time_since_epoch());
    }

    template<typename cast_to = seconds>
    static Timestamp CurrentTimestamp()
    {
        return time_to_uint64<cast_to>(
            high_resolution_clock::now().time_since_epoch());
    }
    static u64 CurrentMicroTimestamp()
    {
        return time_to_uint64<microseconds>(
            high_resolution_clock::now().time_since_epoch());
    }
#endif

    static DateTime GetDateTime(Timestamp)
    {
        return {};
    }

    static CString StringDate(cstring, DateTime)
    {
        return "";
    }
    static CString ClockString()
    {
        return "";
    }

    static Timestamp ParseTimeStdTime(cstring)
    {
        return 0;
    }

    static CString FormattedCurrentTime(cstring)
    {
        return {};
    }
};

struct PosixIshTimeDef : public TimeDef
{
    STATICINLINE Timestamp ParseTimeStdTime(cstring src)
    {
        constexpr cstring fmt    = "%Y-%m-%dT%H:%M:%S";
        struct tm         time_s = {};
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_TIME_FORMAT)
        if(strptime(src, fmt, &time_s))
#elif !defined(COFFEE_NO_TIME_FORMAT)
        std::istringstream ss(src);
        ss.imbue(std::locale(setlocale(LC_ALL, nullptr)));
        ss >> std::get_time(&time_s, fmt);
        if(!ss.fail())
#endif
#if !defined(COFFEE_NO_TIME_FORMAT)
            return mktime(&time_s);
        else
#endif
            return 0;
    }
};

} // namespace Chronology

#if !defined(COFFEE_USE_OLD_TIMERS)
namespace CFunctional {

template<typename time_base, typename clock_type>
class _cbasic_timer_chrono_base : public _cbasic_timer<clock_type>
{
  protected:
    using CLK = Chronology::steady_clock;
    using TP  = Chronology::steady_clock::time_point;

    TP m_start;

    i64 internalElapsed()
    {
        return Chronology::duration_cast<time_base>(CLK::now() - CLK::now())
            .count();
    }

  public:
    void start()
    {
        m_start = CLK::now();
    }
};

class _cbasic_timer_scalar_micro
    : public _cbasic_timer_chrono_base<Chronology::microseconds, bigscalar>
{
  public:
    bigscalar elapsed()
    {
        return internalElapsed() * 1_us;
    }
};

class _cbasic_timer_milli
    : public _cbasic_timer_chrono_base<Chronology::milliseconds, u64>
{
  public:
    u64 elapsed()
    {
        return C_FCAST<u64>(internalElapsed());
    }
};

class _cbasic_timer_micro
    : public _cbasic_timer_chrono_base<Chronology::microseconds, u64>
{
  public:
    u64 elapsed()
    {
        return C_FCAST<u64>(internalElapsed());
    }
};

} // namespace CFunctional
#endif

using TimeDef         = Chronology::TimeDef;
using PosixIshTimeDef = Chronology::PosixIshTimeDef;

#if !defined(COFFEE_USE_OLD_TIMERS)
using CElapsedTimer      = CFunctional::_cbasic_timer_milli;
using CElapsedTimerMicro = CFunctional::_cbasic_timer_micro;
using CElapsedTimerD     = CFunctional::_cbasic_timer_scalar_micro;
#endif

#if defined(COFFEE_STUBBED_TIMING)
using Time = TimeDef;
#endif

} // namespace Coffee

#endif

#include "unix/timers.h"
#include "windows/timers.h"
