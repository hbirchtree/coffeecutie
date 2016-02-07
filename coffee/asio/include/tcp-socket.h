#ifndef COFFEE_ASIO_TCP_SOCKET_H
#define COFFEE_ASIO_TCP_SOCKET_H

#include "asio_data.h"
#include <iostream>

namespace Coffee{
namespace CASIO{

struct TCPSocketImpl : ASIO_Client
{
    using Host = CString;

    using Connection = std::shared_ptr<asio::ip::tcp::socket>;

    STATICINLINE Connection ConnectSocket()
    {

    }
};

}
}

#endif