#ifndef COFFEE_ASIO_TCP_SOCKET_H
#define COFFEE_ASIO_TCP_SOCKET_H

#include "asio_data.h"
#include <iostream>

namespace Coffee{
namespace CASIO{

struct TCPSocketImpl : ASIO_Client
{
    using Host = CString;
    using Service = CString;
    using Port = uint16;

    using Socket = asio::ip::tcp::iostream;

    struct SSLSocket : std::istream, std::ostream
    {
        using sock_t = asio::ssl::stream<asio::ip::tcp::socket>;

        sock_t socket;

        SSLSocket(asio::io_service &serv, asio::ssl::context& ctxt):
            std::istream(&recvp),
            std::ostream(&trans),
            socket(serv,ctxt)
        {
        }

        void connect(AsioContext c, Host h, Service p)
        {
            asio::ip::tcp::resolver::query q(h,p);
            auto it = c->resolver.resolve(q);

            asio::connect(socket.next_layer(),it);

            socket.handshake(asio::ssl::stream_base::client);
        }

        template<typename T>
        void operator<<(T const& v)
        {
            (std::ostream&)(*this) << v;
            flush();
        }

//        template<typename T, typename R>
//        R& operator>>(T& v)
//        {
//            pull();
//            return (std::istream&)(*this) >> v;
//        }

        void pull()
        {
            try{
                asio::read(socket,recvp);
            }catch(std::system_error){}
        }
        void flush()
        {
            std::ostream::flush();
            asio::write(socket,trans);
        }

        void close()
        {
            socket.shutdown();
        }

    private:
        asio::streambuf recvp;
        asio::streambuf trans;
    };
};

}

using TCP = CASIO::TCPSocketImpl;

}

#endif
