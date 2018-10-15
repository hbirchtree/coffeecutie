#pragma once

#include "asio_data.h"

namespace Coffee {
namespace ASIO {

struct UDPSocketImpl : ASIO_Client
{
    using proto    = asio::ip::udp;
    using endpoint = asio::ip::udp::endpoint;

    class Socket : public asio::ip::udp::socket
    {
      public:
        Socket(AsioContext_internal context, proto prot) :
            asio::ip::udp::socket(context->service)
        {
            this->open(prot);
        }

        Socket(AsioContext_internal context, proto prot, uint16 port) :
            asio::ip::udp::socket(context->service)
        {
            this->open(prot);
            this->bind(endpoint(asio::ip::address_v4::any(), port));
        }

        C_DELETE_COPY_CONSTRUCTOR(Socket);
    };
};

} // namespace ASIO

using UDP = ASIO::UDPSocketImpl;

} // namespace Coffee
