#ifndef COFFEE_PLAT_CORE
#define COFFEE_PLAT_CORE

#include "coffee_macros.h"
#include "coffee_types.h"

#ifdef __unix__
#include "unix_core.h"
#elif defined(_WIN32)
#include "win_core.h"
#elif defined(__APPLE__) && defined(__MACH__)

#else
namespace Coffee{

inline static void coffee_enable_core_dump()
{
    C_STUBBED("Core dumping");
}

namespace CFunctional{
namespace CDebugHelpers{
inline static cstring_w* coffee_callstack(szptr *length,uint32 stackreduce = 0)
{
    *length = 0;
    C_STUBBED("Callstack dumping");
    return nullptr;
}
inline static cstring_w coffee_clock_string()
{
    C_STUBBED("Clock strings");
    return "STUBBED";
}
inline static void coffee_clock_free(cstring_w arg)
{
    return;
}

} //CDebugHelpers

//Stub for timers
template<typename T>
class _coffee_stub_timer
{
public:
    void start(){}
    T elapsed(){return 0;}
};
class CElapsedTimer : public _coffee_stub_timer<uint64>{};
class CElapsedTimerMicro : public _coffee_stub_timer<uint64>{};
class CElapsedTimerD : public _coffee_stub_timer<double>{};

} //CFunctional
} //Coffee
#endif


#endif
