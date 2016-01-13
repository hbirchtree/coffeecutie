#include <base/cdebug.h>

#include <plat/plat_core.h>

namespace Coffee{
namespace CDebugPrint{

using namespace CFunctional::CDebugHelpers;

cstring_w coffee_debug_get_clock_string()
{
    return coffee_clock_string();
}

void coffee_debug_clear_clock_string(cstring_w str)
{
    return coffee_clock_free(str);
}

cstring_w *coffee_debug_get_callstack(szptr *cs_length, uint32 stackreduce)
{
    return coffee_callstack(cs_length,stackreduce);
}

}
}
