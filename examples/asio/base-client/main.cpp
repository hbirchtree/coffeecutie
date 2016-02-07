#include <coffee/CCore>
#include <coffee/asio/rest-client.h>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    RestClient::InitService();
    CElapsedTimer tim;

    std::future<RestClient::RestResponse> t =
            RestClient::RestRequestAsync(
                RestClient::GetContext(),
                RestClient::HTTPS,
                "tmi.twitch.tv",
                "/group/user/esl/chatters");
    tim.start();

    cDebug("Launched network task!");

    while(!Threads::FutureAvailable(t));
    cDebug("Results are here: {0}",tim.elapsed());

    RestClient::RestResponse res = t.get();

    cDebug("Status: {0}",res.status);
    cDebug("Header: \n{0}",res.header);
    cDebug("Message: {0}",res.message);
    cDebug("Payload: \n{0}",res.payload);

    JSON::Document doc = JSON::Read(res.payload.c_str());

    if(doc.IsNull())
        return 1;

    cDebug("{0} chatters",doc["chatter_count"].GetInt());
    for(int32 i=0;i<doc["chatters"]["moderators"].Capacity();i++)
    {
        cBasicPrint("{0}",doc["chatters"]["moderators"][i].GetString());
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
