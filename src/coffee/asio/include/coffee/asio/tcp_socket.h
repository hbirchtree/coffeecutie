#pragma once

#include <coffee/core/CProfiling>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/string_casting.h>

#include "asio_data.h"

#define ASIO_TAG "asio::"

#define VALIDATE() \
    if(ec)         \
    return ec

namespace net {

using std::unique_ptr;

using libc_types::u16;
using libc_types::u32;

using context       = Coffee::ASIO::Service;
using context_stats = Coffee::ASIO::Service::stats*;

namespace socket_types {
#if defined(ASIO_USE_SSL)
using ssl     = ::asio::ssl::stream<asio::ip::tcp::socket>;
using udp_ssl = ::asio::ssl::stream<asio::ip::udp::socket>;
#endif
using udp_raw = ::asio::ip::udp::socket;
using raw     = ::asio::ip::tcp::socket;
} // namespace socket_types

using host_t    = std::string;
using service_t = std::string;

namespace detail {

template<typename SocketType>
FORCEDINLINE auto socket_handshake(SocketType&, asio::error_code&)
{
}

#if defined(ASIO_USE_SSL)
template<>
FORCEDINLINE auto socket_handshake(
    socket_types::ssl& socket, asio::error_code& ec)
{
    socket.handshake(socket_types::ssl::handshake_type::client, ec);
}
#endif

} // namespace detail

namespace tcp {

namespace detail {

template<typename SocketType>
FORCEDINLINE auto construct_socket(net::context& service)
{
    return socket_types::raw(service.service);
}

template<typename SocketType>
FORCEDINLINE auto socket_shutdown(SocketType& socket, asio::error_code& ec)
{
    socket.shutdown(socket_types::raw::shutdown_both, ec);
}

#if defined(ASIO_USE_SSL)
template<>
FORCEDINLINE auto construct_socket<socket_types::ssl>(net::context& service)
{
    return socket_types::ssl(service.service, service.sslctxt);
}

template<>
FORCEDINLINE auto socket_shutdown<socket_types::ssl>(
    socket_types::ssl&, asio::error_code&)
{
}
#endif

} // namespace detail

template<typename SocketType>
struct socket_base
{
    using socket_type = SocketType;

    socket_base(net::context& service)
        : m_resolver(service.resolver)
        , m_socket(detail::construct_socket<socket_type>(service))
        , m_stats(service.statistics.get())
    {
    }

    ~socket_base()
    {
        if(m_socket.lowest_layer().is_open())
            disconnect();
    }

    template<typename T>
    asio::error_code connect(host_t const& host, T port)
    {
        asio::error_code ec;

        auto it = m_resolver.resolve(host, stl_types::cast_pod(port), ec);
        VALIDATE();
        asio::connect(m_socket.lowest_layer(), it, ec);
        VALIDATE();
        net::detail::socket_handshake(m_socket, ec);
        m_stats->sockets_created += 1;
        return ec;
    }

    template<typename T>
    auto read(semantic::Span<T>& data, asio::error_code& ec)
    {
        size_t read = 0;
        do
        {
            read += m_socket.read_some(
                asio::buffer(data.data() + read, data.size() - read), ec);
            if(ec || read == data.size())
                break;
        } while(read < data.size());
        m_stats->received += read;
        return read;
    }

    template<typename T>
    auto read_until(
        std::vector<T>&    data,
        std::string const& delim,
        asio::error_code&  ec,
        libc_types::szptr  max_size = 0)
    {
        auto read = asio::read_until(
            m_socket,
            max_size != 0 ? asio::dynamic_buffer(data, max_size)
                          : asio::dynamic_buffer(data),
            asio::string_view(delim.data(), delim.size()),
            ec);
        m_stats->received += read;
        return read;
    }

    template<typename T>
    auto write(semantic::Span<T> const& data, asio::error_code& ec)
    {
        size_t written = 0;
        do
        {
            written += m_socket.write_some(
                asio::buffer(data.data() + written, data.size() - written), ec);
            if(ec || written == data.size())
                break;
        } while(written < data.size());
        m_stats->transmitted += written;
        return written;
    }

    asio::error_code flush()
    {
        return asio::error_code();
    }

    asio::error_code disconnect()
    {
        asio::error_code ec;
        detail::socket_shutdown(m_socket, ec);
        VALIDATE();
        m_socket.lowest_layer().close(ec);
        m_stats->sockets_closed += 1;
        return ec;
    }

    auto& socket()
    {
        return m_socket;
    }

  protected:
    asio::ip::tcp::resolver& m_resolver;
    socket_type              m_socket;
    context_stats            m_stats;
};

#if defined(ASIO_USE_SSL)
struct ssl_socket : socket_base<socket_types::ssl>
{
    using lowest_layer = typename socket_type::lowest_layer_type;
    using socket_base<socket_type>::socket_base;
};
#endif

struct raw_socket : socket_base<socket_types::raw>
{
    using lowest_layer = typename socket_type::lowest_layer_type;
    using socket_base<socket_type>::socket_base;
};

struct server
{
    server(net::context& service, host_t const& host, service_t const& port)
        : m_host(host)
        , m_port(port)
        , m_resolver(service.resolver)
    {
    }

    asio::error_code start()
    {
        asio::error_code ec;

        auto it = m_resolver.resolve(m_host, m_port, ec);
        VALIDATE();
        m_endpoint = *it;
        m_endpoint.port(stl_types::cast_string<u16>(m_port));
        return ec;
    }

  private:
    host_t    m_host;
    service_t m_port;

    asio::ip::tcp::resolver& m_resolver;
    asio::ip::tcp::endpoint  m_endpoint;
};

} // namespace tcp

namespace udp {

enum class write_flags
{
    none = 0,
    eof  = asio::ip::udp::socket::message_end_of_record,
};

enum class protocol
{
    v4,
    v6,
};

template<typename SocketType>
struct socket_base
{
    using socket_type = SocketType;

    socket_base(net::context& service)
        : m_service(service.service)
        , m_resolver(service.resolver_udp)
        , m_socket(service.service)
        , m_stats(service.statistics.get())
    {
    }

    ~socket_base()
    {
        if(m_socket.lowest_layer().is_open())
            close();
    }

    template<typename T>
    asio::error_code connect(
        host_t const& host, T port, protocol proto = protocol::v4)
    {
        using asio::ip::udp;

        asio::error_code ec;

        auto it = m_resolver.resolve(host, stl_types::cast_pod(port), ec);
        VALIDATE();
        m_endpoint = *it;
        m_endpoint.port(port);
        m_stats->sockets_created += 1;
        m_socket.open(
            proto == protocol::v4 ? socket_type::protocol_type::v4()
                                  : socket_type::protocol_type::v6(),
            ec);
        VALIDATE();

        return ec;
    }

    template<typename T>
    asio::error_code connect_broadcast(T port, protocol proto = protocol::v4)
    {
        asio::error_code ec =
            connect(asio::ip::address_v4::broadcast().to_string(), port, proto);
        VALIDATE();
        m_socket.set_option(asio::socket_base::broadcast(true), ec);
        m_socket.set_option(asio::socket_base::reuse_address(true), ec);

        return ec;
    }

    template<typename T>
    asio::error_code listen_on(T port, asio::ip::udp const& flags)
    {
        m_socket = socket_type(m_service, asio::ip::udp::endpoint(flags, port));
        return asio::error_code();
    }

    template<typename T>
    auto read(
        semantic::mem_chunk<T> const& data,
        asio::ip::udp::endpoint&      from,
        asio::error_code&             ec)
    {
        size_t read = 0;
        do
        {
            read += m_socket.receive_from(
                asio::buffer(data.data, data.size), from, 0, ec);
            if(ec || read == data.size)
                break;
        } while(read < data.size);
        m_stats->received += read;
        return read;
    }

    template<typename T>
    auto write(
        semantic::mem_chunk<T> const& data,
        asio::error_code&             ec,
        write_flags                   flags = write_flags::none)
    {
        auto written = write_to(data, m_endpoint, ec, flags);
        m_stats->transmitted += written;
        return written;
    }

    template<typename T>
    auto write_to(
        semantic::mem_chunk<T> const&  data,
        asio::ip::udp::endpoint const& to,
        asio::error_code&              ec,
        write_flags                    flags = write_flags::none)
    {
        size_t written = 0;
        do
        {
            written += m_socket.send_to(
                asio::buffer(data.data, data.size),
                to,
                static_cast<int>(flags),
                ec);
            if(ec || written == data.size)
                break;
        } while(written < data.size);
        m_stats->transmitted += written;
        return written;
    }

    auto& socket()
    {
        return m_socket;
    }

    auto const& endpoint() const
    {
        return m_endpoint;
    }

    asio::error_code close()
    {
        asio::error_code ec;
        m_socket.lowest_layer().close(ec);
        return ec;
    }

  protected:
    asio::io_service&        m_service;
    asio::ip::udp::resolver& m_resolver;
    socket_type              m_socket;
    asio::ip::udp::endpoint  m_endpoint;
    context_stats            m_stats;
};

struct raw_socket : socket_base<socket_types::udp_raw>
{
    using socket_base<socket_type>::socket_base;
};

#if defined(ASIO_USE_SSL)
struct ssl_socket : socket_base<socket_types::udp_ssl>
{
    using socket_base<socket_type>::socket_base;
};
#endif

struct server
{
    server(
        net::context& /*service*/,
        host_t const& /*address*/,
        service_t const& /*port*/)
    {
    }

  private:
    asio::ip::udp::endpoint m_endpoint; /*!< Endpoint to listen to */
};

} // namespace udp

} // namespace net

namespace Coffee::ASIO {

namespace TCP {
using Socket = net::tcp::raw_socket;
#if defined(ASIO_USE_SSL)
using SSLSocket = net::tcp::ssl_socket;
#endif
} // namespace TCP

} // namespace Coffee::ASIO

#undef VALIDATE
#undef ASIO_TAG
