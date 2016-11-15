#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>

using namespace Coffee;

int32 @PJNAME@_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("@PJNAME@/");
    
    cDebug("Hello World!");

    return 0;
}

COFFEE_APPLICATION_MAIN(@PJNAME@_main)
