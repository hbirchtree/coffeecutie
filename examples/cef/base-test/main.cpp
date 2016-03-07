#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/cef/ccef.h>

using namespace Coffee;

CEF::CEFBrowser* browser;
CEF::CEFBrowser* browser2;

void ExitHandle()
{
    CEF::DestroyBrowser(browser);
//    CEF::DestroyBrowser(browser2);
    cDebug("Good shutdown!");
}

int32 coffee_main(int32, cstring_w*)
{
    SubsystemWrapper<CEF::CEFSubsystem> cefsys;
    C_UNUSED(cefsys);

    SetExitFunction(ExitHandle);

    browser = CEF::CreateBrowser();
//    browser2 = CEF::CreateBrowser();

    CEF::SetUrl(browser,"http://youtube.com/");
//    CEF::SetUrl(browser2,"http://leekspin.com/");

    CInput::CIEvent e;
    e.type = CInput::CIEvent::Scroll;
    CInput::CIScrollEvent sev;
    sev.delta.x = 100;

    while(1)
    {
        CEF::CEFSubsystem::Process();
        if(CEF::IsLoaded(browser))
            CEF::ProcessEvent(browser,e,&sev);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
