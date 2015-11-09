#include "cdebug.h"

#include "coffee/core/plat/plat_core.h"

namespace Coffee{
namespace CFunctional{

cstring_w coffee_debug_get_clock_string()
{
    return CDebugHelpers::coffee_clock_string();
}

void coffee_debug_clear_clock_string(cstring_w str)
{
    return CDebugHelpers::coffee_clock_free(str);
}

cstring_w *coffee_debug_get_callstack(szptr *cs_length, uint32 stackreduce)
{
    return CDebugHelpers::coffee_callstack(cs_length,stackreduce);
}

}
}
