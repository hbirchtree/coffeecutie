#pragma once

#if !defined(USE_EMSCRIPTEN_HTTP)
#include <asio.hpp>
#if defined(ASIO_USE_SSL)
#include <asio/ssl.hpp>
#endif
#endif

#include <peripherals/libc/types.h>
#include <peripherals/stl/standard_exceptions.h>

namespace Coffee::ASIO {

#if !defined(USE_EMSCRIPTEN_HTTP)
struct Service
{
    Service() :
        service(), resolver(service), resolver_udp(service),
#if defined(ASIO_USE_SSL)
        sslctxt(asio::ssl::context::sslv23),
#endif
        statistics(std::make_shared<stats>())
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
        libc_types::u32 received        = 0;
        libc_types::u32 transmitted     = 0;
        libc_types::u32 sockets_created = 0;
        libc_types::u32 sockets_closed  = 0;
    };

    std::shared_ptr<stats> statistics;
};

extern std::shared_ptr<ASIO::Service> global_service;
#endif

STATICINLINE auto InitService()
{
#if defined(USE_EMSCRIPTEN_HTTP)
    return 0;
#else
    if(!global_service)
        global_service = std::make_shared<ASIO::Service>();
    return global_service;
#endif
}

} // namespace Coffee::ASIO
