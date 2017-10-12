#include <coffee/CAsio>
#include <coffee/asio/http_parsing.h>
#include <coffee/core/CTiming>
#include <coffee/core/CJSONParser>
#include <coffee/core/CApplication>
#include <coffee/core/CProfiling>
#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/CMD>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    ASIO::AsioContext net_context = ASIO::AsioService::InitService();

    CElapsedTimer tim;

    {
        /* Testing HTTP transport, will return the HTML document */
        ProfContext __m("example.com");

        TCP::Socket cn;
        tim.start();
        cn.connect("example.com","http");

        Profiler::Profile("Connect to host");

        REST::Request request;
        HTTP::InitializeRequest(request);

        request.resource = "/";

        HTTP::GenerateRequest(cn,"example.com",request);

        Profiler::Profile("Generate request");

        cn.flush();
//        cn.pull();

        Profiler::Profile("Flushing socket");

        HTTP::Response resp;

        if(!HTTP::ExtractResponse(cn,&resp))
        {
            cDebug("Bad mojo");
        }else{
            cDebug("Total time: {0}", tim.elapsed());
            for(std::pair<CString,CString> const& v : resp.header)
                cDebug("{0} : {1}",v.first,v.second);
            cDebug("Payload:\n{0}",resp.payload);
        }
        Profiler::Profile("Receive response");
    }

    if(/* DISABLES CODE */false){
        ProfContext __m2("Asynchronous request");

        CString host = "api.twitch.tv";
        REST::Request rq = {};

        HTTP::InitializeRequest(rq);

        rq.transp = REST::HTTPS;
        rq.resource = "/kraken";
        rq.header["Accept"] = "application/vnd.twitchtv.v2+json";
        rq.header["Client-ID"] = {};

        Profiler::Profile("Generate response");

        Function<void(Future<REST::Response>&)> printResult =
                [&](Future<REST::Response>& t)
        {
            while(!Threads::FutureAvailable(t));
            Profiler::Profile("Await response");

            cDebug("Results are here: {0}",tim.elapsed());

            REST::Response res = t.get();
            Profiler::Profile("Response acquisition");

            cDebug("Content type: {0}",REST::GetContentType(res));
            cDebug("Status: {0}",res.status);
            cDebug("Message: {0}",res.message);
            for(auto const& k : res.header)
                cDebug("Header: {0} = {1}", k.first, k.second);

            JSON::Document doc = JSON::Read(res.payload.c_str());

            if(doc.IsNull())
                return;

            cDebug("JSON document:\n{0}",JSON::Serialize(doc));
        };

        Future<REST::Response> t = REST::RestRequestAsync(net_context,host,rq);
        Profiler::Profile("Dispatch request");

        tim.start();
        printResult(t);

        rq.resource = "/kraken/streams";
        t = REST::RestRequestAsync(net_context, host, rq);
        Profiler::Profile("Dispatch request");

        tim.start();
        printResult(t);
    }

    {
        /* UDP socket stuff */

        try {
            UDP::Socket test_socket(net_context, UDP::proto::v4());

            try {
                asio::ip::udp::resolver::query q("localhost", "10240");
                auto it = net_context->resolver_udp.resolve(q);
                CString test_log;
                Profiling::ExportProfilerData(test_log);

//                test_log.resize(4096);

//                while(true)
//                {
//                    Cmd::ReadString(&test_log[0], test_log.size(), stdin);
                    test_socket.send_to(asio::buffer(test_log), *it);
                    cDebug("{0}", Time::Microsecond());
//                }
    //            CString recv;
    //            recv.resize(20);
    //            asio::ip::udp::endpoint endpoint;
    //            test_socket.receive_from(asio::buffer(recv), endpoint);
    //            cDebug("Received {0} bytes:\n{1}", recv.size(), recv);
            } catch (std::system_error const& e) {
                cDebug("Caught exception: {0}", e.what());
            }
            if(test_socket.is_open())
            {
                try{
                    test_socket.close();
                }catch (std::system_error const& e)
                {
                    cDebug("Caught exception: {0}", e.what());
                }
            }
        }catch (std::system_error const& e)
        {
            cDebug("Caught exception: {0}", e.what());
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
