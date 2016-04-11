#ifndef COFFEE_ASIO_ASIO_DATA_H
#define COFFEE_ASIO_ASIO_DATA_H

#include <asio.hpp>
#include <asio/ssl.hpp>

#include <coffee/core/types/tdef/stlfunctypes.h>

#include <coffee/core/CDebug>
#include <coffee/core/coffee_strings.h>

namespace Coffee{
namespace CASIO{

using namespace Threads;

struct ASIO_Client
{
    struct AsioContext_class
    {
        AsioContext_class():
            service(),
            resolver(service),
            sslctxt(asio::ssl::context::sslv23_client)
        {
        }

        asio::io_service service;
        asio::ip::tcp::resolver resolver;
        asio::ssl::context sslctxt;
    };

    using AsioContext = ShPtr<AsioContext_class>;

    STATICINLINE void InitService()
    {
        t_context = AsioContext(new AsioContext_class());
    }

    STATICINLINE AsioContext GetContext()
    {
        AsioContext t = t_context;
        t_context = nullptr;
        return t;
    }

    STATICINLINE bool MakeCurrent(AsioContext c)
    {
        if(t_context.get())
        {
            cLog(__FILE__,__LINE__,CFStrings::ASIO_Library_Name,
                 CFStrings::ASIO_Library_MakeCurrent_Error);
            return false;
        }
        t_context = c;
        return true;
    }

protected:
    static thread_local AsioContext t_context;
};

}
}

#endif
