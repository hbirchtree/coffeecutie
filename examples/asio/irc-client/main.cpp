#include <coffee/CCore>
#include <coffee/CAsio>

using namespace Coffee;

const constexpr cstring quit_message = "Leaving";

TCP::Socket irc_stream;

void ExitFun()
{
    irc_stream << "QUIT :" << quit_message << "\r\n";

    irc_stream.flush();
    irc_stream.close();
}

int32 coffee_main(int32, cstring_w*)
{
    SetExitFunction(ExitFun);

    CString user = "testuser";
    CString channel = "#test";

    irc_stream.connect("localhost","6667");

    irc_stream << "NICK " << user << "\r\n";
    irc_stream << "USER " << user << " 8 * : Sumthing\r\n";

    CString tmp;
    while(std::getline(irc_stream,tmp))
        if(tmp.find("004")>=0)
        {
            cDebug("Logged in");
            break;
        }else if(tmp.find("433")>=0)
        {
            cDebug("Username is taken");
            return 1;
        }

    irc_stream << "JOIN " << channel << "\r\n";

    while(std::getline(irc_stream,tmp))
    {
        if(tmp.find("PING ")==0)
        {
            irc_stream << "PONG " << tmp.substr(5) << "\r\n";
        }else{
            cDebug("{0}",tmp);
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
