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
        TCP::SSLSocket cn;
        cn.connect("example.com","https");

        HTTP::GenerateRequest(cn,"example.com","/",{});

        cn.flush();
        cn.pull();

        HTTP::Response resp;

        if(!HTTP::ExtractResponse(cn,&resp))
        {
            cDebug("Bad mojo");
        }else{
            for(std::pair<CString,CString> const& v : resp.values)
                cDebug("{0} : {1}",v.first,v.second);
            cDebug("Payload:\n{0}",resp.payload);
        }
    }

    TCP::AsioContext c = REST::GetContext();

    CString host = "api.twitch.tv";
    CString rq = "/kraken/streams";

    Threads::Future<REST::RestResponse> t = REST::RestRequestAsync(c,REST::HTTPS,host,rq);

    tim.start();

    cDebug("Launched network task!");

    while(!Threads::FutureAvailable(t));
    cDebug("Results are here: {0}",tim.elapsed());

    REST::RestResponse res = t.get();

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
