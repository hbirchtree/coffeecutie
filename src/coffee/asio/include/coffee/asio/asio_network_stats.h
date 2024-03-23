#pragma once

#include "asio_data.h"
#include <coffee/comp_app/services.h>

namespace Coffee {
namespace ASIO {

struct NetStats
    : comp_app::interfaces::NetworkStatProvider
    , comp_app::AppService<NetStats, comp_app::NetworkStatProvider>
{
    using type = NetStats;

    NetStats([[maybe_unused]] Service& service)
#if !defined(USE_EMSCRIPTEN_HTTP)
        : source(service.statistics)
#endif
    {
    }

#if !defined(USE_EMSCRIPTEN_HTTP)
    std::weak_ptr<Service::stats> source;
#endif

    virtual libc_types::u32 received() const final
    {
#if !defined(USE_EMSCRIPTEN_HTTP)
        if(auto stats = source.lock(); stats)
            return stats->received;
#endif
        return 0;
    }

    virtual libc_types::u32 transmitted() const final
    {
#if !defined(USE_EMSCRIPTEN_HTTP)
        if(auto stats = source.lock(); stats)
            return stats->transmitted;
#endif
        return 0;
    }

    virtual libc_types::u32 connections() const final
    {
#if !defined(USE_EMSCRIPTEN_HTTP)
        if(auto stats = source.lock(); stats)
            return stats->sockets_created;
#endif
        return 0;
    }

    void reset_counters()
    {
#if !defined(USE_EMSCRIPTEN_HTTP)
        if(auto stats = source.lock(); stats)
        {
            stats->received        = 0;
            stats->transmitted     = 0;
            stats->sockets_created = 0;
        }
#endif
    }
};

} // namespace ASIO
} // namespace Coffee
