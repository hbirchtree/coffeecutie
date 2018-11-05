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

struct ASIO_Client
{
    struct AsioContext_data
    {
        AsioContext_data() :
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

        asio::io_service        service;
        asio::ip::tcp::resolver resolver;
        asio::ip::udp::resolver resolver_udp;
#if defined(ASIO_USE_SSL)
        asio::ssl::context sslctxt;
#endif
    };

    using AsioContext_internal = ShPtr<AsioContext_data>;

    STATICINLINE AsioContext_internal InitService()
    {
        return AsioContext_internal(new AsioContext_data());
    }
};

using service      = ASIO_Client;
using asio_context = ASIO_Client::AsioContext_internal;

} // namespace ASIO
} // namespace Coffee
