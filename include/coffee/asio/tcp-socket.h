#ifndef COFFEE_ASIO_TCP_SOCKET_H
#define COFFEE_ASIO_TCP_SOCKET_H

#include "asio_data.h"
#include <iostream>

namespace Coffee{
namespace ASIO{

struct TCPSocketImpl : ASIO_Client
{
    using Socket_ = asio::ip::tcp::iostream;

    /*!
     * \brief A wrapper for asio::ssl::stream to make it work like iostream,
     *  except that it needs to have pull() called before using std::getline.
     * The rest works the same!
     * This is made as a drop-in replacement for Socket, and may not support
     *  all specifics of SSL.
     */
#if defined(ASIO_USE_SSL)
    struct SSLSocket_ : std::istream, std::ostream
    {
        using sock_t = asio::ssl::stream<asio::ip::tcp::socket>;

        AsioContext_internal context;
        sock_t socket;

    private:
        asio::streambuf recvp;
        asio::streambuf trans;

//        SSLSocket_(asio::io_service &serv, asio::ssl::context& ctxt):
//            std::istream(&recvp),
//            std::ostream(&trans),
//            socket(serv,ctxt),
//            recvp(),
//            trans()
//        {
//        }

    public:

        SSLSocket_(AsioContext_internal c):
            std::istream(&recvp),
            std::ostream(&trans),
            context(c),
            socket(c->service,c->sslctxt),
            recvp(),
            trans()
        {
        }

        C_DELETE_COPY_CONSTRUCTOR(SSLSocket_);

        void connect(Host h, Service p)
        {
            asio::ip::tcp::resolver::query q(h,p);
            auto it = context->resolver.resolve(q);
            decltype(it) end;

            if(it == end)
                return;

            asio::connect(socket.next_layer(), it);

            socket.lowest_layer().set_option(
                        asio::ip::tcp::no_delay(true));

            socket.set_verify_mode(asio::ssl::verify_peer);
            socket.set_verify_callback(
                        asio::ssl::rfc2818_verification(h));

            socket.handshake(asio::ssl::stream_base::client);
        }

        template<typename T,typename R>
        R& operator<<(T const& v)
        {
            return (*this) << v;
        }

        template<typename T, typename R>
        R& operator>>(T& v)
        {
            return (*this) >> v;
        }

        asio::error_code pull(szptr* size = nullptr)
        {
            asio::error_code ec;
            auto size_ = asio::read(socket, recvp, ec);
            if(size)
                *size = size_;
            /* Fuck me, std::istream doesn't update itself */
            std::istream::rdbuf(&recvp);
            return ec;
        }
        asio::error_code flush(szptr* size = nullptr)
        {
            std::ostream::flush();
            asio::error_code ec;
            auto size_ = asio::write(socket, trans, ec);
            if(size)
                *size = size_;
            return ec;
        }

        void close()
        {
            socket.shutdown();
            socket.next_layer().close();
        }

    };
#else
    struct SSLSocket_ : public Socket_
    {
        SSLSocket_(AsioContext_internal) {}
        C_DELETE_COPY_CONSTRUCTOR(SSLSocket_);

        std::error_code pull() {return {};}
        std::error_code flush() {return {};}
    };
#endif

    using Socket = Socket_;
    using SSLSocket = SSLSocket_;
};

}

using TCP = ASIO::TCPSocketImpl;

}

#endif
