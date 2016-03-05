#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/cef/ccef.h>

using namespace Coffee;

CEF::CEFBrowser* browser;

void ExitHandle()
{
    CEF::DestroyBrowser(browser);
    cDebug("Good shutdown!");
}

int32 coffee_main(int32, cstring_w*)
{
    SubsystemWrapper<CEF::CEFSubsystem> cefsys;

    SetExitFunction(ExitHandle);

    browser = CEF::CreateBrowser();

    while(1)
    {
        CEF::CEFSubsystem::Process();
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
