#pragma once

#include <coffee/core/CProfiling>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/functional_types.h>

#include "asio_data.h"

#define ASIO_TAG "asio::"

#define VALIDATE() \
    if(ec)         \
    return ec

namespace net {

using stl_types::UqPtr;

using libc_types::u16;
using libc_types::u32;

using context       = Coffee::ASIO::Service;
using context_stats = Coffee::ASIO::Service::stats*;

namespace socket_types {
#if defined(ASIO_USE_SSL)
using ssl = ::asio::ssl::stream<asio::ip::tcp::socket>;
#endif
using raw = ::asio::ip::tcp::socket;
} // namespace socket_types

using host_t    = std::string;
using service_t = std::string;

namespace tcp {

#if defined(ASIO_USE_SSL)
struct ssl_socket
{
    using lowest_layer = socket_types::ssl::lowest_layer_type;

    ssl_socket(net::context& service) :
        m_resolver(service.resolver),
        m_socket(service.service, service.sslctxt),
        m_stats(service.statistics.get())
    {
    }

    asio::error_code connect(host_t const& host, service_t const& port)
    {
        asio::error_code ec;

        auto it = m_resolver.resolve(host, port, ec);
        VALIDATE();
        asio::connect(m_socket.lowest_layer(), it, ec);
        VALIDATE();
        m_socket.handshake(asio::ssl::stream_base::handshake_type::client, ec);
        m_stats->sockets_created += 1;
        return ec;
    }

    template<typename T>
    auto read(semantic::mem_chunk<T>& data, asio::error_code& ec)
    {
        size_t read = 0;
        do
        {
            read += m_socket.read_some(
                asio::buffer(data.data + read, data.size - read), ec);
            if(ec || read == data.size)
                return read;
        } while(read < data.size);
        m_stats->received += read;
        return read;
    }

    template<typename T>
    auto read_until(
        stl_types::Vector<T>&     data,
        stl_types::CString const& delim,
        asio::error_code&         ec,
        libc_types::szptr         max_size = 0)
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
    auto write(semantic::mem_chunk<T> const& data, asio::error_code& ec)
    {
        size_t written = 0;
        do
        {
            written += m_socket.write_some(
                asio::buffer(data.data + written, data.size - written), ec);
            if(ec || written == data.size)
                return written;
        } while(written < data.size);
        m_stats->transmitted += written;
        return written;
    }

    asio::error_code disconnect()
    {
        asio::error_code ec;
        //        m_socket.shutdown(ec);
        //        VALIDATE();
        m_socket.lowest_layer().close(ec);
        return ec;
    }

    asio::error_code flush()
    {
        return asio::error_code();
    }

  private:
    asio::ip::tcp::resolver& m_resolver;
    socket_types::ssl        m_socket;
    context_stats            m_stats;
};
#endif

struct raw_socket
{
    raw_socket(net::context& service) :
        m_resolver(service.resolver), m_socket(service.service),
        m_stats(service.statistics.get())
    {
    }

    asio::error_code connect(host_t const& host, service_t const& port)
    {
        asio::error_code ec;

        auto it = m_resolver.resolve(host, port, ec);
        VALIDATE();
        asio::connect(m_socket, it, ec);

        m_stats->sockets_created += 1;

        return ec;
    }

    template<typename T>
    auto read(semantic::mem_chunk<T>& data, asio::error_code& ec)
    {
        size_t read = 0;
        do
        {
            read += m_socket.read_some(
                asio::buffer(data.data + read, data.size - read), ec);
            if(ec || read == data.size)
                return read;
        } while(read < data.size);
        m_stats->received += read;
        return read;
    }

    template<typename T>
    auto read_until(
        stl_types::Vector<T>&     data,
        stl_types::CString const& delim,
        asio::error_code&         ec,
        libc_types::szptr         max_size = 0)
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
    auto write(semantic::mem_chunk<T> const& data, asio::error_code& ec)
    {
        size_t written = 0;
        do
        {
            written += m_socket.write_some(
                asio::buffer(data.data + written, data.size - written), ec);
            if(ec || written == data.size)
                return written;
        } while(written < data.size);
        m_stats->transmitted += written;
        return written;
    }

    asio::error_code disconnect()
    {
        asio::error_code ec;
        m_socket.shutdown(socket_types::raw::shutdown_both, ec);
        VALIDATE();
        m_socket.close(ec);
        return ec;
    }

    asio::error_code flush()
    {
        return asio::error_code();
    }

  private:
    asio::ip::tcp::resolver& m_resolver;
    socket_types::raw        m_socket;
    context_stats            m_stats;
};

} // namespace tcp

namespace udp {

enum class write_flags
{
    none = 0,
    eof  = asio::ip::udp::socket::message_end_of_record,
};

struct raw_socket
{
    raw_socket(net::context& service) :
        m_resolver(service.resolver_udp), m_socket(service.service),
        m_stats(service.statistics.get())
    {
    }

    asio::error_code connect(host_t const& host, service_t const& port)
    {
        asio::error_code ec;

        auto it = m_resolver.resolve(host, port, ec);
        VALIDATE();
        m_endpoint = *it;
        m_stats->sockets_created += 1;

        return ec;
    }

    asio::error_code connect_broadcast(service_t const& port)
    {
        asio::error_code ec;

        m_socket.set_option(asio::socket_base::broadcast(), ec);
        auto it = m_resolver.resolve(asio::ip::udp::resolver::query(port), ec);
        VALIDATE();

        m_endpoint = asio::ip::udp::endpoint(*it);

        m_stats->sockets_created += 1;

        return ec;
    }

    template<typename T>
    auto read(
        semantic::mem_chunk<T>&  data,
        asio::ip::udp::endpoint& from,
        asio::error_code&        ec)
    {
        size_t read = 0;
        do
        {
            read += m_socket.receive_from(
                asio::buffer(data.data, data.size), from, 0, ec);
            if(ec || read == data.size)
                return read;
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
                return read;
        } while(written < data.size);
        m_stats->transmitted += written;
        return written;
    }

  private:
    asio::ip::udp::resolver& m_resolver;
    asio::ip::udp::socket    m_socket;
    asio::ip::udp::endpoint  m_endpoint;
    context_stats            m_stats;
};

} // namespace udp

} // namespace net

namespace Coffee {
namespace ASIO {

namespace TCP {
using Socket = net::tcp::raw_socket;
#if defined(ASIO_USE_SSL)
using SSLSocket = net::tcp::ssl_socket;
#endif
} // namespace TCP

} // namespace ASIO
} // namespace Coffee

#undef VALIDATE
#undef ASIO_TAG
