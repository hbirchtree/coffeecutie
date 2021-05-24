#pragma once

#include <coffee/asio/tcp_socket.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace natty {

using semantic::mem_chunk;

enum class opcode_t : libc_types::u16
{
    submit_service, /*!< Peers submit their address to allow connecting */
    list_services,  /*!< Peers ask for other peers */

    start_connection, /*!< Sent from peer trying to connect */
    test_connection,  /*!< Test whether NAT punchthrough works for a peer */

    heartbeat, /* For keeping sockets alive */
};

enum class member_t : libc_types::u16
{
    peer   = 0x1,
    server = 0x2,
};

using service_label_t = stl_types::Array<libc_types::byte_t, 64>;

FORCEDINLINE bool assign_label(service_label_t& in, stl_types::String const& to)
{
    if(to.size() >= 64)
        return false;

    std::copy(to.begin(), to.end(), in.begin());
    return true;
}

enum class negotiation_step_t : libc_types::u16
{
    waiting, /* Parked state */

    /* Sending peer information */
    sending_peer_connector,
    sending_peer_target,

    /* Establishing NAT punchthrough */
    client_ping_server,
    server_ping_client,

    cross_the_streams,

    /* Completed state */
    negotiated,
};

FORCEDINLINE negotiation_step_t
             lowest_of(negotiation_step_t v1, negotiation_step_t v2)
{
    auto v1_ = C_CAST<libc_types::u16>(v1), v2_ = C_CAST<libc_types::u16>(v2);

    if(v1_ < v2_)
        return v1;
    else
        return v2;
}

struct alignas(4) service_manifest_t
{
    libc_types::u32 address  = 0;
    libc_types::u16 port     = 0;
    libc_types::u16 _padding = 0;
    service_label_t label    = {};
};

struct alignas(4) connection_manifest_t
{
    struct alignas(4) peer_manifest_t
    {
        libc_types::u32 address;
        libc_types::u16 port;
        libc_types::u16 _padding;

        peer_manifest_t& operator=(service_manifest_t const& from)
        {
            address = from.address;
            port    = from.port;
            return *this;
        }
        peer_manifest_t& operator=(asio::ip::udp::endpoint const& from)
        {
            address = from.address().to_v4().to_uint();
            port    = from.port();
            return *this;
        }
    };

    peer_manifest_t    from, to;
    negotiation_step_t step;
};

struct alignas(4) command_t
{
    opcode_t        opcode;
    libc_types::u16 _padding     = 0;
    libc_types::u32 _reserved[8] = {};

    union opcode_data_t
    {
        struct service_submission_t
        {
            libc_types::u32 address;
            libc_types::u16 port;
            libc_types::u16 _padding;

            service_label_t label;
        } submit;
        struct service_listing_t
        {
            libc_types::u32 count;
        } list;

        connection_manifest_t request;

        struct alignas(4) test_state_t
        {
            negotiation_step_t                     state;
            libc_types::u16                        _padding;
            connection_manifest_t::peer_manifest_t peer;
        } test;

        libc_types::u32 _padding[32] = {};
    } op;
};

struct test_connect_data_t
{
    asio::ip::udp::endpoint originator;
    command_t               msg = {}, res = {};

    negotiation_step_t step = negotiation_step_t::sending_peer_connector;
};

struct peer_connect_data_t
{
    asio::ip::udp::endpoint originator, target;
    command_t               msg[2] = {}, res[2] = {};
    negotiation_step_t      step = negotiation_step_t::sending_peer_connector;

    static peer_connect_data_t waiting()
    {
        peer_connect_data_t out = {};
        out.step                = negotiation_step_t::waiting;

        return out;
    }
};

FORCEDINLINE void asio_noop(asio::error_code ec, size_t){C_ERROR_CHECK(ec)}

FORCEDINLINE void establish_test_connection(
    test_connect_data_t&           data,
    command_t const&               command,
    asio::ip::udp::endpoint const& sender,
    member_t                       member)
{
    data.originator = sender;
    switch(member)
    {
    case member_t::server:
    {
        data.step = negotiation_step_t::waiting;
        break;
    }
    case member_t::peer:
    {
        data.step = negotiation_step_t::sending_peer_connector;
        break;
    }
    }
}

FORCEDINLINE void establish_peer_connection(
    peer_connect_data_t&           data,
    command_t const&               command,
    asio::ip::udp::endpoint const& sender,
    member_t                       member)
{
    data.originator = sender;
    switch(member)
    {
    case member_t::server:
    {
        auto const& target = command.op.request.to;
        data.msg[0].opcode = data.msg[1].opcode = opcode_t::start_connection;
        data.target                             = asio::ip::udp::endpoint(
            asio::ip::address_v4(target.address), target.port);
        data.step = negotiation_step_t::waiting;
        break;
    }
    case member_t::peer:
    {
        data.msg[0].opcode = opcode_t::start_connection;
        data.msg[0].op.request.step =
            negotiation_step_t::sending_peer_connector;

        data.step   = negotiation_step_t::sending_peer_connector;
        data.target = asio::ip::udp::endpoint(
            asio::ip::address_v4(command.op.request.to.address),
            command.op.request.to.port);
        break;
    }
    }
}

FORCEDINLINE void process_test_connection(
    asio::ip::udp::socket& socket, test_connect_data_t& data, member_t member)
{
    data.msg.opcode = opcode_t::test_connection;
    data.step       = data.res.op.test.state;

    switch(member)
    {
    case member_t::server:
    {
        switch(data.step)
        {
        case negotiation_step_t::sending_peer_connector:
        {
            data.step                  = negotiation_step_t::client_ping_server;
            data.msg.op.test.state     = data.step;
            data.msg.op.test.peer.port = socket.local_endpoint().port();
            socket.async_send_to(
                asio::buffer(&data.msg, sizeof(command_t)),
                data.originator,
                asio_noop);
            break;
        }
        case negotiation_step_t::server_ping_client:
        {
            data.step              = negotiation_step_t::negotiated;
            data.msg.op.test.state = data.step;
            socket.async_send_to(
                asio::buffer(&data.msg, sizeof(command_t)),
                data.originator,
                asio_noop);
            break;
        }
        default:
            break;
        }
        break;
    }
    case member_t::peer:
    {
        switch(data.step)
        {
        case negotiation_step_t::client_ping_server:
        {
            data.step              = negotiation_step_t::server_ping_client;
            data.msg.op.test.state = data.step;
            socket.send_to(
                asio::buffer(&data.msg, sizeof(command_t)), data.originator);
            break;
        }
        default:
            break;
        }

        break;
    }
    }
}

FORCEDINLINE void process_peer_connection(
    asio::ip::udp::socket& socket, peer_connect_data_t& data, member_t member)
{
    data.res[0].opcode = data.res[1].opcode = opcode_t::start_connection;
    data.step                               = data.res[0].op.request.step;

    switch(member)
    {
    case member_t::server:
    {
        switch(data.step)
        {
        case negotiation_step_t::sending_peer_connector:
        {
            data.msg[0].op.request.step = data.msg[1].op.request.step =
                negotiation_step_t::client_ping_server;

            connection_manifest_t::peer_manifest_t connector, target;
            target.address    = data.target.address().to_v4().to_uint();
            target.port       = data.target.port();
            connector.address = data.originator.address().to_v4().to_uint();
            connector.port    = data.originator.port();

            data.msg[0].op.request.to   = target;
            data.msg[0].op.request.from = connector;
            data.msg[1].op.request.to   = connector;
            data.msg[1].op.request.from = target;

            socket.async_send_to(
                asio::buffer(&data.msg[0], sizeof(command_t)),
                data.originator,
                asio_noop);
            socket.async_send_to(
                asio::buffer(&data.msg[1], sizeof(command_t)),
                data.target,
                asio_noop);
            break;
        }
        case negotiation_step_t::server_ping_client:
        {
            data.msg[0].op.request.step = data.msg[1].op.request.step =
                negotiation_step_t::cross_the_streams;

            socket.async_send_to(
                asio::buffer(&data.msg[0], sizeof(command_t)),
                data.originator,
                asio_noop);
            socket.async_send_to(
                asio::buffer(&data.msg[1], sizeof(command_t)),
                data.target,
                asio_noop);
            break;
        }
        default:
            break;
        }

        break;
    }
    case member_t::peer:
    {
        data.step = data.res[0].op.request.step;

        switch(data.step)
        {
        case negotiation_step_t::client_ping_server:
        {
            auto const& target = data.res[0].op.request.to;
            data.target        = asio::ip::udp::endpoint(
                asio::ip::address_v4(target.address), target.port);

            data.msg[0] = data.res[0];
            data.msg[0].op.request.step =
                negotiation_step_t::server_ping_client;

            socket.send_to(
                asio::buffer(&data.msg[0], sizeof(command_t)), data.originator);
            break;
        }
        case negotiation_step_t::cross_the_streams:
        {
            data.msg[0].op.request.step = negotiation_step_t::negotiated;

            socket.send_to(
                asio::buffer(&data.msg[0], sizeof(command_t)), data.target);
            break;
        }
        case negotiation_step_t::negotiated:
        {
            command_t msg;
            msg.opcode = opcode_t::heartbeat;
            socket.send_to(asio::buffer(&msg, sizeof(msg)), data.target);
            break;
        }
        default:
            break;
        }

        break;
    }
    }
}

FORCEDINLINE void process_heartbeat(
    asio::ip::udp::socket&         socket,
    asio::ip::udp::endpoint const& sender,
    member_t                       member = member_t::server)
{
    command_t response;
    response.opcode = opcode_t::heartbeat;

    switch(member)
    {
    case member_t::server:
        socket.async_send_to(
            asio::buffer(&response, sizeof(response)), sender, asio_noop);
        break;
    case member_t::peer:
        socket.send_to(asio::buffer(&response, sizeof(response)), sender);
        break;
    }
}

template<typename T>
using active_connections = stl_types::Map<libc_types::u64, T>;

template<typename T>
using connection_initializer = stl_types::Function<void(
    T&, command_t const&, asio::ip::udp::endpoint const&, member_t)>;

template<typename T>
FORCEDINLINE T& get_connection_data(
    active_connections<T>&           connections,
    connection_initializer<T> const& initializer,
    command_t const&                 cmd,
    libc_types::u64                  key,
    asio::ip::udp::endpoint const&   sender,
    member_t                         member)
{
    using libc_types::u64;

    auto it = connections.find(key);
    if(it == connections.end())
    {
        it = connections.insert({key, T()}).first;
        initializer(it->second, cmd, sender, member);
    }

    return it->second;
}

FORCEDINLINE libc_types::u64 endpoint_to_key(
    asio::ip::udp::endpoint const& endpoint)
{
    return endpoint.address().to_v4().to_uint() |
           C_FCAST<libc_types::u64>(endpoint.port()) << 32;
}

FORCEDINLINE libc_types::u64 endpoint_to_key(
    connection_manifest_t::peer_manifest_t const& endpoint)
{
    return endpoint.address | C_FCAST<libc_types::u64>(endpoint.port) << 32;
}

FORCEDINLINE void handle_test_connection(
    asio::ip::udp::socket&                   socket,
    active_connections<test_connect_data_t>& connections,
    command_t&                               command,
    asio::ip::udp::endpoint const&           sender,
    member_t                                 member)
{
    auto& test_data = get_connection_data<test_connect_data_t>(
        connections,
        establish_test_connection,
        command,
        endpoint_to_key(sender),
        sender,
        member);
    test_data.res = command;
    process_test_connection(socket, test_data, member);
}

FORCEDINLINE void handle_peer_connection(
    asio::ip::udp::socket&                   socket,
    active_connections<peer_connect_data_t>& connections,
    command_t&                               command,
    asio::ip::udp::endpoint                  sender,
    member_t                                 member)
{
    auto& req = command.op.request;

    if(req.from.address == 0 || req.from.port == 0)
    {
        req.from.address = sender.address().to_v4().to_uint();
        req.from.port    = sender.port();
    }

    auto& peer_data = get_connection_data<peer_connect_data_t>(
        connections,
        establish_peer_connection,
        command,
        endpoint_to_key(req.to) ^ endpoint_to_key(req.from),
        sender,
        member);
    peer_data.res[0] = command;
    process_peer_connection(socket, peer_data, member);
}

FORCEDINLINE void peer_initiate_test(
    net::udp::raw_socket&          socket,
    test_connect_data_t&           local_data,
    asio::ip::udp::endpoint const& server)
{
    asio::error_code ec;
    command_t        cmd = {};
    cmd.opcode           = opcode_t::test_connection;

    establish_test_connection(local_data, {}, server, member_t::peer);

    socket.write_to(mem_chunk<const command_t>::of(cmd), server, ec);
}

FORCEDINLINE void peer_initiate_connection(
    net::udp::raw_socket&                         socket,
    peer_connect_data_t&                          local_data,
    asio::ip::udp::endpoint const&                server,
    connection_manifest_t::peer_manifest_t const& target)
{
    asio::error_code ec;
    command_t        cmd = {};
    cmd.opcode           = opcode_t::start_connection;
    cmd.op.request.to    = target;
    cmd.op.request.step  = negotiation_step_t::sending_peer_connector;

    establish_peer_connection(local_data, cmd, server, member_t::peer);

    socket.write_to(mem_chunk<const command_t>::of(cmd), server, ec);
    C_ERROR_CHECK(ec)
}

} // namespace natty
