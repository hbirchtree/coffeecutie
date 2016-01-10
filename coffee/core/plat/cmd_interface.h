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

inline cstring ReadString(cstring_w target, int32 size, FILE* strm)
{
    return CGets(target,size,strm);
}

inline void ClearScreen()
{
#if defined(COFFEE_UNIXPLAT)
    printf("\e[1;1H\e[2J");
#endif
}

}

#endif
