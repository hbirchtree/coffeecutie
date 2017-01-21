#include <coffee/CAsio>
#include <coffee/asio/http_parsing.h>
#include <coffee/core/CTiming>
#include <coffee/core/CJSONParser>
#include <coffee/core/CApplication>
#include <coffee/core/CProfiling>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
try{
    ASIO::AsioContext net_context = ASIO::AsioService::InitService();
    Vector<sbyte_t> recv;
    UDP::endpoint remote;
    UDP::Socket sock(net_context, UDP::proto::v4(), 10240);

    recv.resize(4096 * 4);

    std::function<void(asio::error_code const&, size_t)> receive_fun =
            [&](asio::error_code const& code, size_t recv_bytes)
    {
        cDebug("Got {0} bytes, code {1}", recv_bytes, code.message());
        cDebug("Buffer: \n{0}\n{1}",
               Mem::StrUtil::hexdump(recv.data(), recv_bytes),
               C_CAST<cstring>(recv.data()));

        sock.async_receive_from(asio::buffer(recv), remote, receive_fun);
    };

    sock.async_receive_from(asio::buffer(recv), remote, receive_fun);
    while(true)
    {
        net_context->service.run();
    }

    return 0;
}catch(std::system_error const& e)
{
    cDebug("Caught exception: {0}", e.what());
    return 1;
}

COFFEE_APPLICATION_MAIN(coffee_main)
