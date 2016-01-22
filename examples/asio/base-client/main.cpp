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
    fprintf(stderr,"Header: \n%s\n",t.header.c_str());
    cDebug("Message: {0}",t.message);
    cDebug("Payload: {0}",t.payload);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
