#pragma once

#if !defined(COFFEE_WINDOWS) && !defined(COFFEE_NACL) && !defined(GEKKO)
#define USE_ARPA
#include <arpa/inet.h>
#endif

#include "../../types/tdef/integertypes.h"
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Mem{

/* Endian conversion */
FORCEDINLINE u32 ForeignEndian32(u32 i)
{
#ifdef USE_ARPA
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
#ifdef USE_ARPA
    return htons(i);
#else
    /* TODO: Fix this */
    return 0;
#endif
}

FORCEDINLINE u32 NativeEndian32(u32 i)
{
#ifdef USE_ARPA
    return ntohl(i);
#else
    /* TODO: Fix this */
    return 0;
#endif
}

FORCEDINLINE uint16 NativeEndian16(uint16 i)
{
#ifdef USE_ARPA
    return ntohs(i);
#else
    /* TODO: Fix this */
    return 0;
#endif
}

}
}

#undef USE_ARPA
