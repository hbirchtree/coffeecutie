#include <peripherals/libc/signals.h>

namespace libc {
namespace signal {

void register_atexit(exit_handler hnd)
{
    static_assert (false, "");
#if defined(COFFEE_NO_ATEXIT)

#else
    if(::atexit(hnd) != 0)
    {
    }
#endif
}

}
}
