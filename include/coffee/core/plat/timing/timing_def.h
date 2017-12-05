#ifndef COFFEE_CORE_PLAT_TIMING_DEF_H
#define COFFEE_CORE_PLAT_TIMING_DEF_H

#include "../../types/tdef/stltypes.h"
#include "../../types/cdef/timetypes.h"

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

namespace Coffee{
namespace Chronology{

using namespace std::chrono;

template<typename cast_to, typename T>
FORCEDINLINE uint64 time_to_uint64(T d)
{
    return C_FCAST<uint64>(duration_cast<cast_to>(d).count());
}

struct TimeDef
{
    /*!
     * \brief Current microsecond
     * \return
     */
#if defined(COFFEE_USE_CHRONOTIME)
    static uint64 Microsecond()
    {
        return time_to_uint64<microseconds>(
                    high_resolution_clock::now().time_since_epoch()
                    );
    }

    template<typename cast_to = seconds>
    static Timestamp CurrentTimestamp()
    {
        return time_to_uint64<cast_to>(
                    high_resolution_clock::now().time_since_epoch()
                    );
    }
    static uint64 CurrentMicroTimestamp()
    {
        return time_to_uint64<microseconds>(
                    high_resolution_clock::now().time_since_epoch()
                    );
    }
#endif

    static DateTime GetDateTime(Timestamp)
    {
        return {};
    }

    static CString StringDate(cstring,DateTime){return "";}
    static CString ClockString(){return "";}

    static Timestamp ParseTimeStdTime(cstring) { return 0; }

    static CString FormattedCurrentTime(cstring fmt){return {};}
};

struct PosixIshTimeDef : public TimeDef
{

	STATICINLINE Timestamp ParseTimeStdTime(cstring src)
	{
		constexpr cstring fmt = "%Y-%m-%dT%H:%M:%S";
		struct tm time_s = {};
#if defined(COFFEE_UNIXPLAT)
		if (strptime(src, fmt, &time_s))
#elif !defined(COFFEE_GEKKO)
		std::istringstream ss(src);
		ss.imbue(std::locale(setlocale(LC_ALL,nullptr)));
		ss >> std::get_time(&time_s,fmt);
		if(!ss.fail())
#endif
#if !defined(GEKKO)
			return mktime(&time_s);
		else
#endif
			return 0;
	}

};

}

using TimeDef = Chronology::TimeDef;
using PosixIshTimeDef = Chronology::PosixIshTimeDef;

#if defined(COFFEE_STUBBED_TIMING)
using Time = TimeDef;
#endif

}

#endif

#include "windows/timers.h"
#include "unix/timers.h"
