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

    NetStats(Service& service)
        : source(service.statistics)
    {
    }

    std::weak_ptr<Service::stats> source;

    virtual libc_types::u32 received() const final
    {
        if(auto stats = source.lock(); stats)
            return stats->received;
        return 0;
    }

    virtual libc_types::u32 transmitted() const final
    {
        if(auto stats = source.lock(); stats)
            return stats->transmitted;
        return 0;
    }

    virtual libc_types::u32 connections() const final
    {
        if(auto stats = source.lock(); stats)
            return stats->sockets_created;
        return 0;
    }

    void reset_counters()
    {
        if(auto stats = source.lock(); stats)
        {
            stats->received        = 0;
            stats->transmitted     = 0;
            stats->sockets_created = 0;
        }
    }
};

} // namespace ASIO
} // namespace Coffee
