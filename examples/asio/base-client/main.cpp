#include <coffee/CCore>
#include <coffee/asio/include/rest-client.h>

using namespace Coffee;

int32 coffee_main(int32, byte_t**)
{
    CASIO::RestClientImpl::RestResponse t =
            CASIO::RestClientImpl::RestRequest(CASIO::RestClientImpl::HTTP,
                                               "tmi.twitch.tv",
                                               "/group/user/esl/chatters");

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
