#include <coffee/asio/tcp_socket.h>
#include <coffee/core/CApplication>
#include <coffee/strings/libc_types.h>
#include <peripherals/libc/signals.h>

#include <coffee/core/CDebug>

using namespace Coffee;

const constexpr cstring quit_message = "Leaving";

static ASIO::Service context;
static UqPtr<TCP::Socket> irc_stream;

void ExitFun()
{
    if(!irc_stream)
        return;

    *irc_stream << "QUIT :" << quit_message << "\r\n";

    irc_stream->flush();
    irc_stream->sync_close();
}

i32 coffee_main(i32, cstring_w*)
{
    libc::signal::register_atexit(ExitFun);

    CString user    = "testuser";
    CString channel = "#test";

    context = ASIO::ASIO_Client::InitService();
    irc_stream = MkUq<TCP::Socket>(context);

    auto ec = irc_stream->connect(std::chrono::seconds(2), "localhost", "6667");

    if(ec)
    {
        irc_stream = {};
        cWarning("Error occurred: {0}", ec.message());
        return 1;
    }

    *irc_stream << "NICK " << user << "\r\n";
    *irc_stream << "USER " << user << " 8 * : Sumthing\r\n";

    CString tmp;
    while(std::getline(*irc_stream, tmp))
        if(tmp.find("004") == 0)
        {
            cDebug("Logged in");
            break;
        } else if(tmp.find("433") == 0)
        {
            cDebug("Username is taken");
            return 1;
        }

    *irc_stream << "JOIN " << channel << "\r\n";

    while(std::getline(*irc_stream, tmp))
    {
        if(tmp.find("PING ") == 0)
        {
            *irc_stream << "PONG " << tmp.substr(5) << "\r\n";
        } else
        {
            cDebug("{0}", tmp);
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
