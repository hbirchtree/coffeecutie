#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>
#include <coffee/core/CArgParser>
#include <coffee/CAsio>

using namespace Coffee;

int32 coffee_main(int32 argc, cstring_w* argv)
{
    cstring dir_ = ArgParse::Get(argc,argv,"directory");
    cstring cmd_ = ArgParse::Get(argc,argv,"command");
    cstring timer_ = ArgParse::Get(argc,argv,"interval");

    CString dir,cmd;
    uint64 interval = 3600;

    if(!dir_)
        dir = Env::CurrentDir();
    else
        dir = dir_;
    if(!cmd_)
        cmd = cStringFormat("cmake --build {0}",dir);
    else
        cmd = cmd_;
    if(timer_)
    {
        bool ok = false;
        uint64 t__ = Convert::strtoull(timer_,10,&ok);
        if(ok)
            interval = t__;
    }

    cBasicPrint("command={0},directory={1},interval={2}",cmd,dir,interval);

    REST::InitService();

    REST::Request request = "/hbirchtree/coffeecutie/commits/master.atom";

    while(true)
    {
        auto response = REST::RestRequest(REST::HTTPS,"github.com",request);

        if(REST::GetContentType(response) == "application/atom+xml; charset=utf-8")
        {
            cDebug("Updated repository: {0}");
            Proc::Execute(cmd.c_str());
        }

        cDebug("Sleeping again, waking up at {0}",
               Time::LocalTimeString(Time::CurrentTimestamp()+interval*1000));
        Threads::sleepMillis(interval*1000);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main);
