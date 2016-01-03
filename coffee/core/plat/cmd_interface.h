#ifndef COFFEE_CORE_PLAT_COMMAND_INTERFACE_H
#define COFFEE_CORE_PLAT_COMMAND_INTERFACE_H

#include "environment_details.h"
#include <coffee/core/CTypes>

namespace Coffee{

inline bool interactive_cmd()
{
    //TODO: Find way of detecting interactive session
    return false;
}

inline cstring read_string(cstring_w target, int32 size, FILE* strm)
{
    return c_gets(target,size,strm);
}

inline void clear_screen()
{
#if defined(COFFEE_UNIXPLAT)
    printf("\e[1;1H\e[2J");
#endif
}

}

#endif
