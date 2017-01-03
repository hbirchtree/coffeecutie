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
            resolver(service)
#if defined(ASIO_USE_SSL)
          ,
            sslctxt(asio::ssl::context::sslv23_client)
#endif
        {
        }

        asio::io_service service;
        asio::ip::tcp::resolver resolver;
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
