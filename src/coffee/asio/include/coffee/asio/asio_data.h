#pragma once

#include <coffee/core/base.h>

#include <asio.hpp>
#if defined(ASIO_USE_SSL)
#include <asio/ssl.hpp>
#endif

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {
namespace ASIO {

struct Service
{
    Service() :
        service(), resolver(service), resolver_udp(service),
#if defined(ASIO_USE_SSL)
        sslctxt(asio::ssl::context::sslv23),
#endif
        statistics(MkShared<stats>())
    {
#if defined(ASIO_USE_SSL) && !defined(COFFEE_ANDROID)
        asio::error_code ec;
        sslctxt.set_default_verify_paths(ec);
        C_ERROR_CHECK(ec);
//        sslctxt.set_verify_mode(asio::ssl::verify_none, ec);
        C_ERROR_CHECK(ec);
#endif
    }

    ~Service()
    {
    }

    asio::io_service        service;
    asio::ip::tcp::resolver resolver;
    asio::ip::udp::resolver resolver_udp;
#if defined(ASIO_USE_SSL)
    asio::ssl::context sslctxt;
#endif

    struct stats
    {
        u32 received = 0;
        u32 transmitted = 0;
        u32 sockets_created = 0;
    };

    ShPtr<stats> statistics;
};

extern ShPtr<ASIO::Service> global_service;

STATICINLINE ShPtr<ASIO::Service> InitService()
{
    if(!global_service)
        global_service = MkShared<ASIO::Service>();
    return global_service;
}

} // namespace ASIO
} // namespace Coffee
