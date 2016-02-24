#include <coffee/core/CApplication>
#include <coffee/core/CMD>

using namespace Coffee;

int32 coffee_main(int32,cstring_w*)
{
    cDebug("Press any key to exit");
    Cmd::ReadString(nullptr,0,stdin);
    getchar();
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)