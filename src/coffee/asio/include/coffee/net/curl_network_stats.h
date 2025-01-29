#pragma once

#include <coffee/comp_app/services.h>

#include "curl_context.h"

namespace net {

struct CurlNetStats
    : comp_app::interfaces::NetworkStatProvider
    , comp_app::AppService<CurlNetStats, comp_app::NetworkStatProvider>
{
    CurlNetStats(std::shared_ptr<curl_data> curl_context)
        : m_context(curl_context)
    {
    }

    libc_types::u32 received() const
    {
        return m_context->stats.received;
    }
    libc_types::u32 transmitted() const
    {
        return m_context->stats.transmitted;
    }
    libc_types::u32 connections() const
    {
        return m_context->stats.connections;
    }
    void reset_counters()
    {
        m_context->stats = {};
    }

private:
    std::shared_ptr<curl_data> m_context;
};

}
