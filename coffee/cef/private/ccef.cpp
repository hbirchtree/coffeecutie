#include <coffee/cef/ccef.h>

#include <coffee/CImage>
#include <coffee/core/CMD>
#include <coffee/core/argument_handling.h>

#include <include/base/cef_scoped_ptr.h>
#include <include/cef_render_handler.h>
#include <include/cef_base.h>
#include <include/cef_client.h>
#include <include/cef_dom.h>
#include <include/cef_v8.h>
#include <include/cef_values.h>
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"

namespace Coffee{
namespace CEF{

using namespace CInput;
using namespace Display;

static AppArg m_args;
static CefMainArgs cef_args;

/* TODO: Make this modifiable, to set directory paths and etc. */
static CefSettings cef_settings;

bool CEFSubsystem::Init()
{
    m_args = AppArg::Clone(GetInitArgs());
    cef_args = CefMainArgs(m_args.argc,m_args.argv);

    cDebug("CEF init");

    CefSettings& settings = cef_settings;
    cef_string_utf16_t path_string = {};
    cstring subpath = "./CoffeeCefSlave";
    cef_string_ascii_to_utf16(subpath,StrLen(subpath),&path_string);
    settings.browser_subprocess_path = path_string;

    return CefInitialize(cef_args,settings,nullptr,nullptr);
}

bool CEFSubsystem::Deinit()
{
    CefShutdown();
    return true;
}

bool CEFSubsystem::Process()
{
    CefDoMessageLoopWork();
    return true;
}

class CEFOffscreenBrowser : public CefRenderHandler
{
    // CefRenderHandler interface
public:
    CEFOffscreenBrowser()
    {
    }

    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override
    {
        rect.Set(0,0,m_size.width,m_size.height);
        return true;
    }
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                 const RectList &dirtyRects, const void *buffer,
                 int width, int height) override
    {
        texture_out.bpp = 4;
        texture_out.size.w = width;
        texture_out.size.h = height;
        texture_out.data = (byte_t*)buffer;

        CResources::Resource browser_bitm(outfile.c_str());

        CStbImageLib::SavePNG(&browser_bitm,&texture_out);

        CResources::FileCommit(browser_bitm);
        CResources::FileFree(browser_bitm);
    }

    CefSize m_size;
    CString outfile;

    CefBrowserSettings bsettings;
private:
    CStbImageLib::CStbImage texture_out;

    IMPLEMENT_REFCOUNTING(CEFOffscreenBrowser);
};

class CEFCoffeeClient : public CefClient
{
public:
    CEFCoffeeClient(CEFOffscreenBrowser* b):
        m_handle(b)
    {
    }

    CefRefPtr<CEFOffscreenBrowser> m_handle;

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override
    {
        return m_handle;
    }

    IMPLEMENT_REFCOUNTING(CEFCoffeeClient)
};

struct CEFBrowser
{
    CEFOffscreenBrowser* renderhandle;
    CEFCoffeeClient* client;
    CefRefPtr<CefBrowser> browser;
};

CEFBrowser* CreateBrowser(cstring start_url)
{
    CefWindowInfo winfo;
    winfo.SetAsWindowless(0,true);

    CEFBrowser* b = new CEFBrowser();
    CEFOffscreenBrowser* osr = new CEFOffscreenBrowser();
    osr->bsettings.windowless_frame_rate = 60;
    osr->m_size.Set(1024,768);
    osr->outfile = cStringFormat("CEFBrowser_{0}.png",(const void* const&)osr);

    b->renderhandle = osr;
    b->client = new CEFCoffeeClient(osr);
    b->browser = CefBrowserHost::CreateBrowserSync(
                winfo,b->client,"http://www.google.com",
                b->client->m_handle.get()->bsettings,nullptr);
    return b;
}

void DestroyBrowser(CEFBrowser* b)
{
    delete b;
}

void SetUrl(CEFBrowser *b, cstring url)
{
    CefString m;
    m.FromASCII(url);
    b->browser->GetMainFrame()->LoadURL(m);
}

void PerformAction(CEFBrowser *b, BAction action)
{
    switch(action)
    {
    case BAction::Back:
        b->browser->GoBack();
        break;
    case BAction::Forward:
        b->browser->GoForward();
        break;
    case BAction::Refresh:
        b->browser->Reload();
        break;
    case BAction::Stop:
        b->browser->StopLoad();
        break;
    default:
        return;
    }
}

void ProcessEvent(CEFBrowser *b, const CIEvent &e, c_cptr data)
{
    switch(e.type)
    {
    case CIEvent::Keyboard:
    {
        auto kev = (const CIKeyEvent*)data;
        CefKeyEvent ev;
        ev.character = kev->key;
        ev.type = feval(kev->mod&CIKeyEvent::PressedModifier)
                ? KEYEVENT_KEYDOWN
                : KEYEVENT_KEYUP;
        b->browser->GetHost()->SendKeyEvent(ev);
        break;
    }
    case CIEvent::MouseMove:
    {
        auto mev = (const CIMouseMoveEvent*)data;
        CefMouseEvent ev;
        ev.x = mev->origin.x;
        ev.y = mev->origin.y;

        ev.modifiers |= feval(mev->btn&CIMouseButtonEvent::LeftButton)
                ? EVENTFLAG_LEFT_MOUSE_BUTTON : 0;
        ev.modifiers |= feval(mev->btn&CIMouseButtonEvent::MiddleButton)
                ? EVENTFLAG_MIDDLE_MOUSE_BUTTON : 0;
        ev.modifiers |= feval(mev->btn&CIMouseButtonEvent::RightButton)
                ? EVENTFLAG_RIGHT_MOUSE_BUTTON : 0;

        b->browser->GetHost()->SendMouseMoveEvent(ev,false);
        break;
    }
    case CIEvent::MouseButton:
    {
        auto mev = (const CIMouseButtonEvent*)data;
        CefMouseEvent ev;
        ev.x = mev->pos.x;
        ev.y = mev->pos.y;

        CefBrowserHost::MouseButtonType k = MBT_LEFT;

        if(feval(mev->btn&CIMouseButtonEvent::LeftButton))
            k = MBT_LEFT;
        else if(feval(mev->btn&CIMouseButtonEvent::RightButton))
            k = MBT_RIGHT;
        else if(feval(mev->btn&CIMouseButtonEvent::MiddleButton))
            k = MBT_MIDDLE;
        else break;

        b->browser->GetHost()->SendMouseClickEvent(
                    ev,k,
                    feval(mev->mod&CIMouseButtonEvent::Pressed),
                    1);
        break;
    }
    case CIEvent::Scroll:
    {
        auto sev = (const CIScrollEvent*)data;

        CefMouseEvent ev;
        ev.x = 0;
        ev.y = 0;

        b->browser->GetHost()->SendMouseWheelEvent(ev,sev->delta.x,sev->delta.y);
        break;
    }
    default:
        break;
    }

}

void ProcessEvent(CEFBrowser *b, const CDEvent &e, c_cptr data)
{
    switch(e.type)
    {
    case CDEvent::Focus:
    {
        auto fev = (const CDFocusEvent*)data;
        b->browser->GetHost()->SendFocusEvent(feval(fev->mod&CDFocusEvent::Enter));
        break;
    }
    case CDEvent::Resize:
    {
        auto rev = (const CDResizeEvent*)data;
        Resize(b,*rev);
        break;
    }
    default:
        break;
    }
}

void Resize(CEFBrowser *b, CSize const& size)
{
    b->renderhandle->m_size.width = size.w;
    b->renderhandle->m_size.height = size.h;
    b->browser->GetHost()->WasResized();
}

void AwaitLoaded(CEFBrowser *b)
{
    while(b->browser->IsLoading());
}

bool IsLoaded(CEFBrowser *b)
{
    return !b->browser->IsLoading();
}

}
}
