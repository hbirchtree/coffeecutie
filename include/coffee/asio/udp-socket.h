#pragma once

#include "asio_data.h"

namespace Coffee{
namespace ASIO{

struct UDPSocketImpl : ASIO_Client
{
    using Socket = asio::ip::udp::socket;
};

}

using UDP = ASIO::UDPSocketImpl;

}
