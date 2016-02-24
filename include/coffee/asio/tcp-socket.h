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

    using Connection = std::shared_ptr<asio::ip::tcp::socket>;

    STATICINLINE Connection ConnectSocket(Host host, Service service)
    {
	asio::ip::tcp::resolver::query q(host,service);
        Connection conn;
        try{
            auto resolve_iterator = t_context->resolver.resolve(q);
            conn = Connection(new asio::ip::tcp::socket(t_context->service));
            asio::connect(*conn,resolve_iterator);
        }catch(std::system_error)
        {
            conn = Connection();
        }

	return conn;
    }

    STATICINLINE std::future<Connection> ConnectSocketAsync(
	    AsioContext ctxt, Host const& host, Service const& service)
    {
	std::function<Connection()> fun = [ctxt,host,service]()
	{
	    MakeCurrent(ctxt);
	    Connection conn = ConnectSocket(host,service);
	    GetContext();
	    return conn;
	};
        return Threads::RunAsync(fun);
    }

    STATICINLINE std::future<bool> DisconnectSocket(Connection *c)
    {
	std::function<bool()> fun = [c]()
	{
	    if(!c->unique())
		return false;
	    c->get()->close();
	    c->reset();
	    return true;
	};
        return Threads::RunAsync(fun);
    }
};

}

using TCPSocket = CASIO::TCPSocketImpl;

}

#endif
