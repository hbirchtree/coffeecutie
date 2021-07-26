#pragma once

#include <peripherals/identify/system.h>

//#include "power.h"

//#include "android/sysinfo.h"
//#include "emscripten/sysinfo.h"
//#include "linux/sysinfo.h"
//#include "osx/sysinfo.h"
//#include "raspberry/sysinfo.h"
//#include "win32/sysinfo.h"

//#include "base/sysinfo.h"

#include "linux/sysinfo.h"

namespace platform::info {

namespace proc {

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
using linux_::node_count;
using linux_::cpu_count;
using linux_::core_count;
using linux_::thread_count;

using linux_::model;
using linux_::frequency;

#else

inline u32 cpu_count()
{
    return 1;
}
inline u32 core_count()
{
    return 1;
}
inline u32 thread_count(u32 cpu = 0)
{
    return std::thread::hardware_concurrency();
}

#endif

inline bool is_hyperthreaded()
{
    return core_count() != thread_count();
}

}

namespace memory {

using linux_::stats;
using linux_::virtual_stats;

}

namespace os {

#if defined(COFFEE_LINUX)
using linux_::kernel;
using linux_::kernel_version;
using linux_::architecture;

#elif defined(COFFEE_WINDOWS)
using win32::kernel;
using win32::kernel_version;
using linux_::architecture;

using win32::is_wow64;
using win32::wine_version;

#else
#error System info not implemented
#endif

}

namespace device {

#if defined(COFFEE_ANDROID)
using android::device;
using android::motherboard;

#elif defined(COFFEE_EMSCRIPTEN)
using emscripten::device;

#elif defined(COFFEE_LINUX)
using linux_::device;

using linux_::motherboard;
using linux_::chassis;

#else
#error System info not implemented
#endif

}

}
