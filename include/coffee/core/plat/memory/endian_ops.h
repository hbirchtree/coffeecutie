#pragma once

#ifndef COFFEE_WINDOWS
#include <arpa/inet.h>
#endif

#include "../../types/tdef/integertypes.h"
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Mem{

/* Endian conversion */
FORCEDINLINE uint32 ForeignEndian32(uint32 i)
{
#ifndef COFFEE_WINDOWS
    return htonl(i);
#else
    return  ((i >> 24) & 0x000000FF) |
            ((i >> 8) & 0x0000FF00) |
            ((i << 8) & 0x00FF0000) |
            ((i << 24) & 0xFF000000);
#endif
}

FORCEDINLINE uint16 ForeignEndian16(uint16 i)
{
#ifndef COFFEE_WINDOWS
    return htons(i);
#else
    /* TODO: Fix this */
    return 0;
#endif
}

FORCEDINLINE uint32 NativeEndian32(uint32 i)
{
#ifndef COFFEE_WINDOWS
    return ntohl(i);
#else
    /* TODO: Fix this */
    return 0;
#endif
}

FORCEDINLINE uint16 NativeEndian16(uint16 i)
{
#ifndef COFFEE_WINDOWS
    return ntohs(i);
#else
    /* TODO: Fix this */
    return 0;
#endif
}

}
}
