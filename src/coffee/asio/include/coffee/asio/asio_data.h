#pragma once

#include <coffee/core/base.h>

#include <asio.hpp>
#if defined(ASIO_USE_SSL)
#include <asio/ssl.hpp>
#endif

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {
/*!
 * \brief ASIO bindings with simplified structures
 */
namespace ASIO {

struct Service
{
    Service() :
        service(), resolver(service), resolver_udp(service)
#if defined(ASIO_USE_SSL)
        ,
        sslctxt(asio::ssl::context::sslv23)
#endif
    {
#if defined(ASIO_USE_SSL) && !defined(COFFEE_ANDROID)
        asio::error_code ec;
        sslctxt.set_default_verify_paths(ec);
        if(ec != asio::error_code())
        {
            throw undefined_behavior(
                "Setting verification paths failed: " + ec.message());
        }
        sslctxt.set_verify_mode(asio::ssl::verify_peer);
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
};

STATICINLINE ShPtr<ASIO::Service> InitService()
{
    return MkShared<ASIO::Service>();
}

} // namespace ASIO
} // namespace Coffee
