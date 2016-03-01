#include <coffee/CCore>
#include <coffee/CAsio>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    RestClient::InitService();
    CElapsedTimer tim;

    CASIO::ASIO_Client::AsioContext c = RestClient::GetContext();

    {
        std::future<TCPSocket::Connection> conn =
                TCPSocket::ConnectSocketAsync(c,"google.com","80");

        TCPSocket::Connection cn = conn.get();

        cDebug("Socket: {0}",(const void* const&)cn.get());

        TCPSocket::DisconnectSocket(&cn);
    }

    std::future<RestClient::RestResponse> t =
            RestClient::RestRequestAsync(
                c,
                RestClient::HTTPS,
                "api.guildwars2.com",
                "/v2");

    tim.start();

    cDebug("Launched network task!");

    while(!Threads::FutureAvailable(t));
    cDebug("Results are here: {0}",tim.elapsed());

    RestClient::RestResponse res = t.get();

    cDebug("Content type: {0}",RestClient::GetContentType(res));

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
