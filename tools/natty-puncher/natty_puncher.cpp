#include "natty_puncher.h"
#include <coffee/asio/tcp_socket.h>
#include <coffee/core/CApplication>
#include <coffee/core/task_queue/task.h>
#include <peripherals/semantic/chunk.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

using namespace natty;
using namespace Coffee;

static ShPtr<ASIO::Service> service;

void client_connector(ASIO::Service& service, String const& host)
{
    using namespace std::chrono_literals;

    net::udp::raw_socket test_listener(service);
    C_ERROR_CHECK(test_listener.connect(host, 42069))

    auto server = service.resolver_udp.resolve(host, "42069")->endpoint();

    cDebug(
        "Socket created, server is {0}:{1}",
        server.address().to_string(),
        server.port());

    test_connect_data_t test_data;
    peer_connect_data_t peer_data;

    peer_data.originator = server;

    asio::ip::udp::endpoint sender;
    asio::error_code        ec;

    connection_manifest_t::peer_manifest_t manifest;
    {
        cDebug("Enumerating services");
        command_t cmd;
        cmd.opcode = opcode_t::list_services;

        test_listener.write_to(semantic::Bytes::From(cmd), server, ec);
        C_ERROR_CHECK(ec)
        test_listener.read(semantic::Bytes::From(cmd), sender, ec);

        for(C_UNUSED(auto i) : Range<>(cmd.op.list.count))
        {
            service_manifest_t manifest_;
            test_listener.read(semantic::Bytes::From(manifest_), sender, ec);
            C_ERROR_CHECK(ec)
            manifest = manifest_;
        }
    }

    {
        cDebug("Submitting service");
        command_t cmd;
        cmd.opcode = opcode_t::submit_service;

        assign_label(cmd.op.submit.label, CurrentThread::GetName());
        test_listener.write_to(semantic::Bytes::From(cmd), server, ec);
        test_listener.read(semantic::Bytes::From(cmd), sender, ec);
    }

    if(manifest.port == 0)
    {
        cDebug("Starting connection test");
        peer_initiate_test(test_listener, test_data, server);
    } else
    {
        cDebug("Starting peer connection");
        peer_initiate_connection(test_listener, peer_data, server, manifest);
    }

    command_t command;
    while(test_listener.socket().is_open())
    {
        test_listener.read(semantic::Bytes::From(command), sender, ec);
        C_ERROR_CHECK(ec)
        cDebug(
            "Got message {0} from {1}:{2}",
            C_CAST<u32>(command.opcode),
            sender.address().to_string(),
            sender.port());
        switch(command.opcode)
        {
        case opcode_t::heartbeat:
            process_heartbeat(test_listener.socket(), sender, member_t::peer);
            break;
        case opcode_t::test_connection:
            test_data.res = command;
            cDebug("  Pre processing: {0}", C_CAST<u32>(test_data.step));
            process_test_connection(
                test_listener.socket(), test_data, member_t::peer);
            cDebug("  Post processing: {0}", C_CAST<u32>(test_data.step));
            break;
        case opcode_t::start_connection:
            peer_data.res[0] = command;
            cDebug("  Pre connection state: {0}", C_CAST<u32>(peer_data.step));
            process_peer_connection(
                test_listener.socket(), peer_data, member_t::peer);
            cDebug("  Post connection state: {0}", C_CAST<u32>(peer_data.step));
            cDebug(
                "  Current target: {0}:{1}, current server: {2}:{3}",
                peer_data.target.address().to_string(),
                peer_data.target.port(),
                peer_data.originator.address().to_string(),
                peer_data.originator.port());
            break;
        default:
            break;
        }
    }

    cDebug("Closing thread");
}

i32 natty_main()
{
    using namespace std::chrono_literals;

    service = ASIO::InitService();

    {
        runtime_queue_error rqec;
        auto tester1 = RuntimeQueue::CreateNewThreadQueue("Target", rqec);
        auto tester2 = RuntimeQueue::CreateNewThreadQueue("Connector", rqec);

        auto connector_task = []() { client_connector(*service, "birchy.dev"); };

        RuntimeQueue::QueueShot(tester1, 2s, connector_task, rqec);
        C_ERROR_CHECK(rqec)
        RuntimeQueue::QueueShot(tester2, 5s, connector_task, rqec);
        C_ERROR_CHECK(rqec)
    }

    CurrentThread::SetName("Server");

    asio::error_code     ec;
    net::udp::raw_socket cmd_socket(*service);
    ec = cmd_socket.listen_on(42069, asio::ip::udp::v4());
    C_ERROR_CHECK(ec)

    std::vector<service_manifest_t> services;

    command_t command;
    auto      command_ref = semantic::Bytes::From(command);

    natty::active_connections<natty::test_connect_data_t> test_connections;
    natty::active_connections<natty::peer_connect_data_t> peer_connections;

    Function<void(asio::error_code, size_t)> handle;
    asio::ip::udp::endpoint                  sender;

    handle = [&handle,
              &services,
              &cmd_socket,
              &sender,
              &command,
              &test_connections,
              &peer_connections](asio::error_code ec, size_t) {
        if(ec)
        {
            cWarning("Failure on receive: {0}", ec.message());
            return;
        }

        cDebug(
            "New message from {0}:{1} = {2}",
            sender.address().to_string(),
            sender.port(),
            C_CAST<u32>(command.opcode));

        switch(command.opcode)
        {
        case opcode_t::submit_service:
        {
            services.push_back({});
            auto& manifest   = services.back();
            manifest.address = sender.address().to_v4().to_uint();
            manifest.port    = sender.port();
            manifest.label   = command.op.submit.label;

            cmd_socket.socket().async_send_to(
                asio::buffer(&command, sizeof(command_t)), sender, asio_noop);

            break;
        }
        case opcode_t::list_services:
        {
            command.opcode        = opcode_t::list_services;
            command.op.list.count = services.size();
            cmd_socket.socket().async_send_to(
                asio::buffer(&command, sizeof(command_t)), sender, asio_noop);

            for(auto const& service : services)
                cmd_socket.socket().async_send_to(
                    asio::buffer(&service, sizeof(service_manifest_t)),
                    sender,
                    asio_noop);
            break;
        }
        case opcode_t::test_connection:
        {
            natty::handle_test_connection(
                cmd_socket.socket(),
                test_connections,
                command,
                sender,
                member_t::server);
            break;
        }
        case opcode_t::start_connection:
        {
            auto const& target = command.op.request.to;
            cDebug(
                "Connection info:\n"
                "Endpoint = {0}:{1}\n"
                "Target = {2}:{3}\n"
                "State = {4}",
                sender.address().to_string(),
                sender.port(),
                asio::ip::address_v4(target.address).to_string(),
                target.port,
                C_CAST<u32>(command.op.request.step));

            natty::handle_peer_connection(
                cmd_socket.socket(),
                peer_connections,
                command,
                sender,
                member_t::server);

            for(auto const& peer : peer_connections)
                cDebug(
                    "New state: {1} = {0}",
                    C_CAST<u32>(peer.second.step),
                    peer.first >> 32);
            break;
        }
        case opcode_t::heartbeat:
        {
            natty::process_heartbeat(cmd_socket.socket(), sender);
            break;
        }
        default:
            break;
        }

        cmd_socket.socket().async_receive_from(
            asio::buffer(&command, sizeof(command_t)), sender, handle);
    };

    cmd_socket.socket().async_receive_from(
        asio::buffer(&command, sizeof(command_t)), sender, handle);

    service->service.run();

    return 0;
}

COFFEE_APPLICATION_MAIN(natty_main)
