#include <coffee/cef/ccef.h>

#include <coffee/CImage>
#include <coffee/core/CMD>

#include <include/base/cef_logging.h>
#include <include/base/cef_scoped_ptr.h>
#include <include/cef_app.h>
#include <include/cef_command_line.h>
#include <include/wrapper/cef_helpers.h>

#include <include/cef_render_handler.h>
#include <include/cef_base.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_dom.h>
#include <include/cef_v8.h>
#include <include/cef_values.h>

#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"

#include <build/libs/cef_binary_3.2641.1392.ge1aa8cc_linux64/include/cef_render_handler.h>
#include <build/libs/cef_binary_3.2641.1392.ge1aa8cc_linux64/include/cef_base.h>
#include <build/libs/cef_binary_3.2641.1392.ge1aa8cc_linux64/include/cef_browser.h>
#include <build/libs/cef_binary_3.2641.1392.ge1aa8cc_linux64/include/cef_client.h>
#include <build/libs/cef_binary_3.2641.1392.ge1aa8cc_linux64/include/cef_dom.h>
#include <build/libs/cef_binary_3.2641.1392.ge1aa8cc_linux64/include/cef_v8.h>
#include <build/libs/cef_binary_3.2641.1392.ge1aa8cc_linux64/include/cef_values.h>

namespace Coffee{
namespace CEF{

static AppArg m_args;
static CefMainArgs cef_args;

/* TODO: Make this modifiable, to set directory paths and etc. */
static CefSettings cef_settings;

bool CEFSubsystem::Init()
{
    m_args = AppArg::Clone(GetInitArgs());
    cef_args = CefMainArgs(m_args.argc,m_args.argv);

    int res = CefExecuteProcess(cef_args,nullptr,nullptr);

    if(res >= 0)
        Cmd::Exit(0);
    else if(res == -1)
    {
        CefSettings& settings = cef_settings;

        return CefInitialize(cef_args,settings,nullptr,nullptr);
    }
}

bool CEFSubsystem::Deinit()
{
    CefShutdown();
    return true;
}

bool CEFSubsystem::Process()
{
    CefDoMessageLoopWork();
}

class CEFOffscreenBrowser : public CefRenderHandler
{
    // CefRenderHandler interface
public:
    CEFOffscreenBrowser()
    {
    }

    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
    {
        rect.Set(0,0,800,600);
        return true;
    }
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                 const RectList &dirtyRects, const void *buffer,
                 int width, int height)
    {
        texture_out.bpp = 4;
        texture_out.size.w = width;
        texture_out.size.h = height;
        texture_out.data = (byte_t*)buffer;

        CResources::CResource browser_bitm("browser.png");

        CStbImageLib::SavePNG(&browser_bitm,&texture_out);

        CResources::FileCommit(browser_bitm);
        CResources::FileFree(browser_bitm);

        CStbImageLib::ImageFree(&texture_out);
    }

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

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler()
    {
        return m_handle;
    }

    IMPLEMENT_REFCOUNTING(CEFCoffeeClient);
};

struct CEFBrowser
{
    CEFCoffeeClient* client;
    CefRefPtr<CefBrowser> browser;
};

CEFBrowser* CreateBrowser()
{
    CefWindowInfo winfo;
    winfo.SetAsWindowless(0,true);

    CEFBrowser* b = new CEFBrowser();
    CEFOffscreenBrowser* osr = new CEFOffscreenBrowser();
    osr->bsettings.windowless_frame_rate = 60;

    b->client = new CEFCoffeeClient(osr);
    b->browser = CefBrowserHost::CreateBrowserSync(
                winfo,b->client,"http://example.com",
                b->client->m_handle.get()->bsettings,nullptr);
    return b;
}
void DestroyBrowser(CEFBrowser* b)
{
    delete b;
}

}
}