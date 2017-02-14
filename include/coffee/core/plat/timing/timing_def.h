#ifndef COFFEE_CORE_PLAT_TIMING_DEF_H
#define COFFEE_CORE_PLAT_TIMING_DEF_H

#include "../../types/tdef/stltypes.h"
#include "../../types/cdef/timetypes.h"

#include "../plat_primary_identify.h"

#include <ctime>
#include <time.h>

#if defined(COFFEE_WINDOWS) || defined(COFFEE_NACL)
#include <iomanip>
#include <sstream>
#endif

namespace Coffee{

struct TimeDef
{
    /*!
     * \brief Current microsecond
     * \return
     */
    static uint64 Microsecond(){return 0;}

    static Timestamp CurrentTimestamp(){return 0;}
    static uint64 CurrentMicroTimestamp(){return 0;}

    static DateTime GetDateTime(Timestamp){return DateTime();}

    static CString StringDate(cstring,DateTime){return "";}
    static CString ClockString(){return "";}

	static Timestamp ParseTimeStdTime(cstring) { return 0; }
};

struct PosixIshTimeDef
{

	STATICINLINE Timestamp ParseTimeStdTime(cstring src)
	{
		constexpr cstring fmt = "%Y-%m-%dT%H:%M:%S";
		struct tm time_s = {};
#if !defined(COFFEE_WINDOWS) && !defined(COFFEE_NACL)
		if (strptime(src, fmt, &time_s))
#else
		std::istringstream ss(src);
		ss.imbue(std::locale(setlocale(LC_ALL,nullptr)));
		ss >> std::get_time(&time_s,fmt);
		if(!ss.fail())
#endif
			return mktime(&time_s);
		else
			return 0;
	}

};

}

#endif

#include "windows/timers.h"
#include "unix/timers.h"
