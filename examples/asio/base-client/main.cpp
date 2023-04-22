#include <coffee/asio/http.h>
#include <coffee/asio/net_resource.h>
#include <coffee/asio/tcp_socket.h>
#include <coffee/asio/udp_socket.h>
#include <coffee/core/CApplication>
#include <coffee/core/CJSONParser>
#include <coffee/core/CProfiling>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/url.h>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;

i32 coffee_main(i32, cstring_w*)
{
    ASIO::asio_context net_context;

    runtime_queue_error ec;
    auto q        = RuntimeQueue::CreateNewThreadQueue("Networking", ec);
    u64  net_task = 0;

    {
        ProfContext _("Init context");
        net_context = ASIO::service::InitService();

        C_ERROR_CHECK(ec);

        net_task = RuntimeQueue::QueueImmediate(
            q,
            Chrono::milliseconds(0),
            [&]() {
                auto work = asio::make_work_guard(net_context->service);

                net_context->service.run();

                cDebug("Worker loop: {0}", &work.get_executor().context());
            },
            ec);

        C_ERROR_CHECK(ec);
    }

    if(false)
    {
        //        /* Testing HTTP transport, will return the HTML document */
        //        ProfContext __m("example.com");

        //        TCP::Socket cn;
        //        tim.start();
        //        cn.connect("example.com", "http");

        //        Profiler::Profile("Connect to host");

        //        REST::Request request;
        //        HTTP::InitializeRequest(request);

        //        request.resource = "/";

        //        HTTP::GenerateRequest(cn, "example.com", request);

        //        Profiler::Profile("Generate request");

        //        cn.flush();
        //        //        cn.pull();

        //        Profiler::Profile("Flushing socket");

        //        HTTP::Response resp;

        //        if(!HTTP::ExtractResponse(cn, &resp))
        //        {
        //            cDebug("Bad mojo");
        //        } else
        //        {
        //            cDebug("Total time: {0}", tim.elapsed());
        //            for(std::pair<std::string, std::string> const& v : resp.header)
        //                cDebug("{0} : {1}", v.first, v.second);
        //            cDebug("Payload:\n{0}", (cstring)resp.payload.data());
        //        }
        //        Profiler::Profile("Receive response");
    }

    /*
#if defined(ASIO_USE_SSL)
    {
        using namespace http::header;
        using http::content_type;
        using http::header_field;

        ProfContext _("Manual HTTP request");

        TCP::SSLSocket   connection(net_context);
        concurrent_notif var;

        auto get_response = [&](size_t) {
            http::header_t response_header =
                http::stream::read_response(connection);

            cDebug(
                "{0} {1} {2}",
                response_header.code,
                http::header::to_string::version(response_header.version),
                response_header.message);

            for(auto h : response_header.fields)
                cDebug("{0}: {1}", h.first, h.second);

            auto size_it = response_header.standard_fields.find(
                header_field::content_length);

            if(size_it != response_header.standard_fields.end())
            {
                auto len = cast_string<u64>(size_it->second);

                connection.async_read_some(
                    len,
                    {[&var, &connection, len](size_t some) {

                         cDebug("Content-Length: {0}, got {1}", len, some);

                         http::payload_t p =
                             http::stream::read_payload(connection, len);

                         cDebug(
                             "{0}: {1}",
                             p.size(),
                             str::encapsulate(p.data(), p.size()));

                         var.notify();
                     },
                     [](asio::error_code) {
                         cWarning("Failed to read payload");
                     }});
            }
        };

        auto pull_response = [&]() {
            connection.async_read_until(
                http::header_terminator, {get_response, [](asio::error_code) {
                                              cWarning("Failed to pull");
                                          }});
        };

        //        TCP::Socket connection;
        auto make_request = [&]() {
            using connect_pol = http::connection_policy;

            http::request_t req;
            req.host            = "coffee.birchtrees.me";
            req.port            = 443;
            req.header.resource = "/";
            req.header.version  = http::version_t::v11;
            req.header.method   = http::method_t::get;

            auto& st_fields = req.header.standard_fields;

            st_fields[header_field::accept] =
                to_string::content_type(content_type::image);
            st_fields[header_field::user_agent] = "coffee/1.0";
            st_fields[header_field::connection] =
                to_string::connection_policy(connect_pol::close);

            auto header =
                serialize::request(transform::create_request(req).header);

            cDebug("Header: \n{0}", header);

            connection << header;
            connection.async_write({pull_response, [](asio::error_code) {
                                        cWarning("Failed to write request");
                                    }});
        };

        {
            var.prepare_lock();

            connection.async_connect(
                {make_request,
                 [](asio::error_code) { cWarning("Failed to connect"); }},
                false,
                "coffee.birchtrees.me",
                "https");

            var.await();
        }

        connection.sync_close();
    }
#endif
*/
    for(C_UNUSED(auto i) : Range<>(10))
    {
        TCP::Socket connection(net_context);

        auto ec =
            connection.connect(std::chrono::seconds(2), "example.com", "http");

        if(!ec)
            connection.sync_close();
    }

    {
        //        using asio::ip::tcp;
        //        namespace ssl = asio::ssl;
        //        typedef ssl::stream<tcp::socket> ssl_socket;

        //        // Create a context that uses the default paths for
        //        // finding CA certificates.
        //        ssl::context ctx(ssl::context::sslv23);
        //        ctx.set_default_verify_paths();

        //        // Open a socket and connect it to the remote host.
        //        asio::io_service io_service;
        //        ssl_socket sock(io_service, ctx);
        //        tcp::resolver resolver(io_service);
        //        tcp::resolver::query query("hookb.in", "https");
        //        asio::connect(sock.lowest_layer(), resolver.resolve(query));
        //        sock.lowest_layer().set_option(tcp::no_delay(true));

        //        // Perform SSL handshake and verify the remote host's
        //        // certificate.
        //        sock.set_verify_mode(ssl::verify_peer);
        //        sock.set_verify_callback(ssl::rfc2818_verification("hookb.in"));
        //        sock.handshake(ssl_socket::client);

        //        asio::streambuf buf;
        //        std::ostream os(&buf);

        //        HTTP::Request r;

        //        HTTP::InitializeRequest(r);

        //        r.resource = "/Ekqw9wW0";
        //        r.reqtype = "POST";

        //        r.payload = {'A', 'B', 'C', 'D'};

        //        HTTP::GenerateRequest(os, "hookb.in", r);

        //        os.flush();

        //        auto s = asio::write(sock, buf);

        //        cDebug("Wrote: {0}", s);

        //        asio::streambuf rbuf;
        //        std::istream is(&rbuf);

        //        asio::error_code ec;
        //        asio::read(sock, rbuf, ec);

        //        cDebug("Error: {0}",  ec.message());

        //        is.rdbuf(&rbuf);
    }

    {
        Url           testUrl = "https://i.imgur.com/nQdOmCJ.png"_web;
        Net::Resource testRsc(net_context, testUrl);

        if(testRsc.fetch())
        {
            cDebug("Success!");
            Bytes data = testRsc.data();
            cDebug("Data size: {0} {1}", data.size, data.data[0]);
        } else
            cWarning("Failed imgur");
    }

    net_context->service.stop();

    RuntimeQueue::AwaitTask(q->threadId(), net_task, ec);

    return 0;

    if(/* DISABLES CODE */ (false))
    {
        Url           testUrl = "https://hookb.in/Ekqw9wW0"_web;
        Net::Resource testRsc(net_context, testUrl);

        cstring test = "random string";

        Bytes data;
        data = Bytes::CreateString(test);

        if(testRsc.push(http::method_t::post, data))
        {
            auto d = testRsc.data();
            cDebug("Hello!: \n{0}", str::encapsulate((cstring)d.data, d.size));
        } else
            cWarning("Failed hookbin");
    }

#ifndef COFFEE_LOWFAT
    {
        /* UDP socket stuff */

        try
        {
            UDP::Socket test_socket(net_context, UDP::proto::v4());

            try
            {
                asio::ip::udp::resolver::query q("localhost", "10240");
                auto    it = net_context->resolver_udp.resolve(q);
                std::string test_log;
                Profiling::ExportProfilerData(test_log);

                //                test_log.resize(4096);

                //                while(true)
                //                {
                //                    Cmd::ReadString(&test_log[0],
                //                    test_log.size(), stdin);
                test_socket.send_to(asio::buffer(test_log), *it);
                cDebug("{0}", Time<>::Microsecond());
                //                }
                //            std::string recv;
                //            recv.resize(20);
                //            asio::ip::udp::endpoint endpoint;
                //            test_socket.receive_from(asio::buffer(recv),
                //            endpoint); cDebug("Received {0} bytes:\n{1}",
                //            recv.size(), recv);
            } catch(std::system_error const& e)
            {
                cDebug("Caught exception: {0}", e.what());
            }
            if(test_socket.is_open())
            {
                try
                {
                    test_socket.close();
                } catch(std::system_error const& e)
                {
                    cDebug("Caught exception: {0}", e.what());
                }
            }
        } catch(std::system_error const& e)
        {
            cDebug("Caught exception: {0}", e.what());
        }
    }
#endif

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
