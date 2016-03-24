#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/coffee.h>
#include <coffee/cef/ccef.h>

using namespace Coffee;

/* We store pointers to the browser instances here */
CEF::CEFBrowser* browser;
CEF::CEFBrowser* browser2;

/* This is the panic handler */
void ExitHandle()
{
    CEF::DestroyBrowser(browser);
//    CEF::DestroyBrowser(browser2);
    cDebug("Good shutdown!");
}

int32 coffee_main(int32, cstring_w*)
{
    /* Start the CEF subsystem, which launches daemon processes */
    SubsystemWrapper<CEF::CEFSubsystem> cefsys;
    C_UNUSED(cefsys);

    /* We create a panic handler, in case everything goes to shit. */
    /* In this case, CEF exits poorly. */
    SetExitFunction(ExitHandle);

    /* We create a browser instance, of which we can create many */
    browser = CEF::CreateBrowser();
//    browser2 = CEF::CreateBrowser();

    /* We can set the browser URL */
    CEF::SetUrl(browser,"http://youtube.com/");
//    CEF::SetUrl(browser2,"http://leekspin.com/");

    /* We can transmit events! */
    CInput::CIEvent e;
    e.type = CInput::CIEvent::Scroll;
    CInput::CIScrollEvent sev;
    sev.delta.x = 100;

    while(1)
    {
        /* In order to keep CEF instances active, they need some processing. */
        CEF::CEFSubsystem::Process();
        /* While the browser is active, we hand it events */
        if(CEF::IsLoaded(browser))
            CEF::ProcessEvent(browser,e,&sev);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
