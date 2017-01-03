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

    public:
        SSLSocket_(asio::io_service &serv, asio::ssl::context& ctxt):
            std::istream(&recvp),
            std::ostream(&trans),
            socket(serv,ctxt),
            recvp(),
            trans()
        {
        }

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

            asio::connect(socket.next_layer(),it);

            socket.handshake(asio::ssl::stream_base::client);
        }

        template<typename T,typename R>
        R& operator<<(T const& v)
        {
            std::ostream& ref = *this;
            std::ostream& r = ref << v;
            flush();
            return r;
        }

        template<typename T, typename R>
        R& operator>>(T& v)
        {
            pull();
            std::istream& ref = *this;
            return ref >> v;
        }

        void pull()
        {
#ifndef NDEBUG
            try{
#endif
                asio::read(socket,recvp);
#ifndef NDEBUG
            }catch(std::system_error){}
#endif
            /* Fuck me, std::istream doesn't update itself */
            std::istream::rdbuf(&recvp);
        }
        void flush()
        {
            std::ostream::flush();
            asio::write(socket,trans);
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

        void pull() {}
    };
#endif

    using Socket = Socket_;
    using SSLSocket = SSLSocket_;
};

}

using TCP = ASIO::TCPSocketImpl;

}

#endif
