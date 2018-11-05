#pragma once

#include <peripherals/libc/types.h>

#if defined(COFFEE_WINDOWS)
#include <coffee/core/plat/plat_windows.h>
#elif defined(COFFEE_GEKKO)
#include <network.h>
#else
#include <arpa/inet.h>
#endif

namespace endian {
using namespace Coffee::Primitives;

struct net_order
{
};

struct host_order
{
};

template<typename Type, typename Direction>
struct conversion_param
{
    using direction = Direction;
    using type      = Type;
};

using u16_host = conversion_param<u16, host_order>;
using u16_net  = conversion_param<u16, net_order>;

using u32_host = conversion_param<u32, host_order>;
using u32_net  = conversion_param<u32, net_order>;

#define ENDIAN_TEMPLATE(out_type, order)                               \
    template<                                                          \
        typename Param,                                                \
        typename std::enable_if<                                       \
            std::is_same<typename Param::type, out_type>::value &&     \
                std::is_same<typename Param::direction, order>::value, \
            Param>::type* = nullptr>

ENDIAN_TEMPLATE(u16, host_order)
FORCEDINLINE typename Param::type to(typename Param::type v)
{
    return ntohs(v);
}

ENDIAN_TEMPLATE(u16, net_order)
FORCEDINLINE typename Param::type to(typename Param::type v)
{
    return htons(v);
}

ENDIAN_TEMPLATE(u32, host_order)
FORCEDINLINE typename Param::type to(typename Param::type v)
{
    return ntohl(v);
}

ENDIAN_TEMPLATE(u32, net_order)
FORCEDINLINE typename Param::type to(typename Param::type v)
{
    return htonl(v);
}

#undef ENDIAN_TEMPLATE

} // namespace endian
