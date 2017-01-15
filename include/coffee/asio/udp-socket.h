#pragma once

#include "asio_data.h"

namespace Coffee{
namespace ASIO{

struct UDPSocketImpl : ASIO_Client
{
    class Socket : public asio::ip::udp::socket
    {
    public:

        using UDP = asio::ip::udp;

        Socket(AsioContext_internal context) :
            asio::ip::udp::socket(context->service)
        {
        }

        C_DELETE_COPY_CONSTRUCTOR(Socket);
    };
};

}

using UDP = ASIO::UDPSocketImpl;

}
