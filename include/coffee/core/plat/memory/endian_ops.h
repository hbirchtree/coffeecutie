#pragma once

#ifndef COFFEE_WINDOWS
#include <arpa/inet.h>
#else
//#include <WinSock2.h>
#endif

#include "../../types/tdef/integertypes.h"
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Mem{

/* Endian conversion */
#ifndef COFFEE_WINDOWS
FORCEDINLINE uint32 ForeignEndian32(uint32 i)
{
    return htonl(i);
}

FORCEDINLINE uint16 ForeignEndian16(uint16 i)
{
    return htons(i);
}

FORCEDINLINE uint32 NativeEndian32(uint32 i)
{
    return ntohl(i);
}

FORCEDINLINE uint16 NativeEndian16(uint16 i)
{
    return ntohs(i);
}
#endif

}
}