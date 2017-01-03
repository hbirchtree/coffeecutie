#ifndef COFFEE_ASIO_ASIO_DATA_H
#define COFFEE_ASIO_ASIO_DATA_H

#include <asio.hpp>
#include <asio/ssl.hpp>

#include <coffee/core/types/tdef/stlfunctypes.h>

#include <coffee/core/CDebug>
#include <coffee/core/CThreading>
#include <coffee/core/coffee_strings.h>

namespace Coffee{
namespace ASIO{

struct ASIO_Client
{
    struct AsioContext_data
    {
        AsioContext_data():
            service(),
            resolver(service),
            sslctxt(asio::ssl::context::sslv23_client)
        {
        }

        asio::io_service service;
        asio::ip::tcp::resolver resolver;
        asio::ssl::context sslctxt;
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
