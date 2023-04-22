#include <peripherals/libc/signals.h>

#include <peripherals/stl/types.h>

namespace libc {
namespace signal {

std::vector<exit_handler> global_exit_handlers;

void register_atexit(exit_handler hnd)
{
#if defined(COFFEE_NO_ATEXIT)
    global_exit_handlers.push_back(hnd);
#else
    if(::atexit(hnd) != 0)
    {
    }
#endif
}

}
}
