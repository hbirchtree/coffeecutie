#include <peripherals/libc/signals.h>

namespace libc {
namespace signal {

void register_atexit(exit_handler hnd)
{
#if defined(COFFEE_NO_ATEXIT)

#else
    if(::atexit(hnd) != 0)
    {
    }
#endif
}

}
}
