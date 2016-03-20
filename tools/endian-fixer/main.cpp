#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>

using namespace Coffee;

int32 coffee_main(int32 argc, cstring_w* argv)
{
    if(argc!=3)
        return 1;

    cstring fn1 = argv[1];
    cstring fn2 = argv[2];

    cBasicPrint("Process {0} -> {1}",fn1,fn2);
    cBasicPrintNoNL("Press any key to proceed...");
    Cmd::Wait();
    cBasicPrint("Boom!");

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main);