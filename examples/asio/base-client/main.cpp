#include <coffee/CAsio>
#include <coffee/asio/http_parsing.h>
#include <coffee/core/CTiming>
#include <coffee/core/CJSONParser>
#include <coffee/core/CApplication>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    REST::InitService();
    CElapsedTimer tim;

    {
        ProfContext __m("example.com");

        TCP::SSLSocket cn;
        cn.connect("example.com","https");

        Profiler::Profile("Connect to host");

        REST::Request request;
        HTTP::InitializeRequest(request);

        request.resource = "/";

        HTTP::GenerateRequest(cn,"example.com",request);

        Profiler::Profile("Generate request");

        cn.flush();
        cn.pull();

        Profiler::Profile("Flushing socket");

        HTTP::Response resp;

        if(!HTTP::ExtractResponse(cn,&resp))
        {
            cDebug("Bad mojo");
        }else{
            for(std::pair<CString,CString> const& v : resp.values)
                cDebug("{0} : {1}",v.first,v.second);
            cDebug("Payload:\n{0}",resp.payload);
        }
        Profiler::Profile("Receive response");
    }

    ProfContext __m2("Asynchronous request");

    TCP::AsioContext c = REST::GetContext();
    Profiler::Profile("Acquire ASIO context");

    CString host = "api.twitch.tv";
    REST::Request rq = {};

    HTTP::InitializeRequest(rq);

    rq.transp = REST::HTTPS;
    rq.resource = "/kraken/streams";
    rq.values["Accept"] = "application/json";

    Profiler::Profile("Generate response");

    Threads::Future<REST::RestResponse> t = REST::RestRequestAsync(c,host,rq);
    Profiler::Profile("Dispath request");

    tim.start();

    cDebug("Launched network task!");

    while(!Threads::FutureAvailable(t));
    Profiler::Profile("Await response");

    cDebug("Results are here: {0}",tim.elapsed());

    REST::RestResponse res = t.get();
    Profiler::Profile("Resposne acquisition");

    cDebug("Content type: {0}",REST::GetContentType(res));

    cDebug("Status: {0}",res.status);
    cDebug("Header: \n{0}",res.header);
    cDebug("Message: {0}",res.message);
    cDebug("Payload: \n{0}",res.payload);

    JSON::Document doc = JSON::Read(res.payload.c_str());

    if(doc.IsNull())
        return 1;

    cDebug("JSON document:\n{0}",JSON::Serialize(doc));

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
