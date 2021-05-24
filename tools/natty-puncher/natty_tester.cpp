#include "natty_puncher.h"
#include <coffee/asio/tcp_socket.h>
#include <coffee/core/CApplication>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

using namespace natty;
using namespace Coffee;

i32 natty_main()
{
    auto service = ASIO::InitService();

    asio::error_code     ec;
    net::udp::raw_socket test_listener(*service);
    C_ERROR_CHECK(test_listener.connect("192.168.50.10", 42069))

    cDebug("Set up socket");

    command_t               command;
    auto                    cmd_ref = mem_chunk<command_t>::of(command);
    asio::ip::udp::endpoint sender, server = test_listener.endpoint();

    cDebug("Server is: {0}:{1}", server.address().to_string(), server.port());

    command.opcode = opcode_t::list_services;
    test_listener.write_to(cmd_ref, server, ec);
    test_listener.read(cmd_ref, sender, ec);

    service_manifest_t manifest;
    {
        auto man_ref = mem_chunk<service_manifest_t>::of(manifest);
        cDebug("Detected {0} services:", command.op.list.count);
        for(C_UNUSED(auto i) : Range<>(command.op.list.count))
        {
            test_listener.read(man_ref, sender, ec);
            cDebug(
                " - {0} = {1}:{2}",
                C_RCAST<cstring>(manifest.label.data()),
                asio::ip::address_v4(manifest.address).to_string(),
                manifest.port);
        }
    }

    cDebug("Submitting self to services");

    command.opcode = opcode_t::submit_service;
    assign_label(command.op.submit.label, "Test Service");

    test_listener.write_to(cmd_ref, server, ec);
    test_listener.read(cmd_ref, sender, ec);

    test_connect_data_t test_data;
    peer_connect_data_t peer_data;

    if(manifest.port != 0)
    {
        cDebug("Starting connection");
        establish_peer_connection(peer_data, command, server, member_t::peer);
        command.opcode          = opcode_t::start_connection;
        command.op.request.step = negotiation_step_t::sending_peer_connector;
        command.op.request.to   = manifest;
    } else
    {
        cDebug("Testing connection");
        command.opcode        = opcode_t::test_connection;
        command.op.test.state = negotiation_step_t::sending_peer_connector;

        establish_peer_connection(peer_data, {}, server, member_t::peer);
        peer_data.step = negotiation_step_t::waiting;
    }

    test_listener.write_to(cmd_ref, server, ec);

    establish_test_connection(test_data, {}, server, member_t::peer);

    while(test_listener.socket().is_open())
    {
        test_listener.read(mem_chunk<command_t>::of(command), sender, ec);
        C_ERROR_CHECK(ec)
        cDebug("Got message {0} from {1}", C_CAST<u32>(command.opcode),
               sender.address().to_string());
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

        //        process_heartbeat(test_listener.socket(), server,
        //        member_t::peer);
    }

    //    cDebug("Sending opcode");
    //    test_listener.write(cmd_ref, ec);

    //    cDebug("Reading from server");
    //    asio::ip::udp::endpoint server;
    //    test_listener.read(cmd_ref, server, ec); /* SYNC 1 */
    //    C_ERROR_CHECK(ec)

    //    cDebug("Negotiate connection");
    //    auto remote_host = "birchy.dev";
    //    C_ERROR_CHECK(ec)

    //    cDebug(
    //        "Connecting to {0}:{1}",
    //        remote_host,
    //        command.op.test.peer.port);

    //    auto server_test = server;
    //    server_test.port(6969);

    //    test_listener.write_to(cmd_ref, server_test, ec); /* W1 */

    //    cDebug("Wrote initial handshake");

    //    test_listener.read(cmd_ref, server, ec); /* R2 */

    //    cDebug("Reading back");

    //    auto client = test_listener.socket().local_endpoint();
    //    cDebug(
    //        "Connected {0}:{1} -> {2}:{3}",
    //        client.address().to_string(),
    //        client.port(),
    //        server.address().to_string(),
    //        server.port());

    //    Array<char, 8> test_data;
    //    auto test_data_ref = semantic::Bytes::CreateFrom(test_data);
    //    asio::ip::udp::endpoint last;
    //    test_listener.read(test_data_ref, last, ec);

    //    cDebug("Last message: {0}", test_data.data());

    return 0;
}

COFFEE_APPLICATION_MAIN(natty_main)
