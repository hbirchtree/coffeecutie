#include <coffee/core/CApplication>
#include <coffee/core/CFiles>

#include <coffee/core/CDebug>

using namespace Coffee;

int32 @PJNAME@_main(int32, cstring_w*)
{
    cDebug("Hello World!");

    return 0;
}

COFFEE_APPLICATION_MAIN(@PJNAME@_main)
