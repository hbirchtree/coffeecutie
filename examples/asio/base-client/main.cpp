#include <coffee/CCore>
#include <coffee/asio/include/rest-client.h>

using namespace Coffee;

int32 coffee_main(int32, byte_t**)
{
    RestClient::InitService();
    RestClient::RestResponse t =
            RestClient::RestRequest(RestClient::HTTP,
                                    "tmi.twitch.tv",
                                    "/group/user/esl/chatters");

    cDebug("Status: {0}",t.status);
    cDebug("Header: {0}",t.header);
    cDebug("Message: {0}",t.message);
    cDebug("Payload: \n{0}",t.payload);

    JSON::Document doc = JSON::Read(t.payload.c_str());

    cDebug("{0} chatters",doc["chatter_count"].GetInt());
    for(int32 i=0;i<doc["chatters"]["moderators"].Capacity();i++)
    {
        cBasicPrint("{0}",doc["chatters"]["moderators"][i].GetString());
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
