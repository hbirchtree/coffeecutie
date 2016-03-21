#include <coffee/CAsio>
#include <coffee/CCore>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    REST::InitService();
    CElapsedTimer tim;

    {
        TCP::SSLSocket cn;
        cn.connect("example.com","https");

        cn << "GET / HTTP/1.0\r\n";
        cn << "HOST: example.com\r\n";
        cn << "CONNECTION: close\r\n\r\n";

        CString data;

        CString tmp;

        cn.pull();

        std::getline(cn,tmp);
        cDebug("Response: {0}",tmp);
        if(tmp.substr(0,5) == "HTTP/")
        {
            while(std::getline(cn,tmp) && tmp != "\r")
                cDebug("{0}",tmp);

            while(std::getline(cn,tmp) && tmp != "\r\n")
            {
                data.append(tmp);
            }

            cDebug("Payload: \n{0}",data);
        }else{
            cDebug("Bad mojo");
        }
    }

    TCP::AsioContext c = REST::GetContext();

    CString host = "api.twitch.tv";
    CString rq = "/kraken/streams";

    std::future<REST::RestResponse> t = REST::RestRequestAsync(c,REST::HTTPS,host,rq);

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

    cDebug("{0} chatters",doc["chatter_count"].GetInt());
    for(int32 i=0;i<doc["chatters"]["viewers"].Capacity();i++)
    {
        cBasicPrint("{0}",doc["chatters"]["viewers"][i].GetString());
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
