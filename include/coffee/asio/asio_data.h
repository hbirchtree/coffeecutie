#ifndef COFFEE_ASIO_ASIO_DATA_H
#define COFFEE_ASIO_ASIO_DATA_H

#include <coffee/core/plat/plat_quirks_toggling.h>

#include <asio.hpp>
#if defined(ASIO_USE_SSL)
#include <asio/ssl.hpp>
#endif

#include <coffee/core/types/tdef/stlfunctypes.h>

#include <coffee/core/CDebug>
#include <coffee/core/CThreading>
#include <coffee/core/coffee_strings.h>

namespace Coffee{
/*!
 * \brief ASIO bindings with simplified structures
 */
namespace ASIO{

struct ASIO_Client
{
    using Host = CString;
    using Service = CString;
    using Port = uint16;

    struct AsioContext_data
    {
        AsioContext_data():
            service(),
            resolver(service),
            resolver_udp(service)
#if defined(ASIO_USE_SSL)
          ,sslctxt(asio::ssl::context::sslv23)
#endif
        {
#if defined(ASIO_USE_SSL) && !defined(COFFEE_ANDROID)
            asio::error_code ec;
            sslctxt.set_default_verify_paths(ec);
            if(ec != asio::error_code())
            {
                cWarning("Setting verification paths failed: {0}",
                         ec.message());
            }
            sslctxt.set_verify_mode(asio::ssl::verify_peer);
#endif
        }

        asio::io_service service;
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

using AsioContext = ASIO_Client::AsioContext_internal;
using AsioService = ASIO_Client;

}
}

#endif
