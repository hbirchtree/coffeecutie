#include <coffee/core/display/cdialog_fun.h>

namespace CoffeeExt{
namespace QtDialogs{

using namespace Coffee;

struct QtSplash : Coffee::SplashInterface
{
    struct SplashHandle;

    static SplashHandle* CreateSplash();

    static void ShowSplash(SplashHandle* s);
    static void HideSplash(SplashHandle* s);

    static void SetSize(SplashHandle* s,CSize const&size);

    static void SetTitle(SplashHandle*,Title const& title);
    static void SetSubText(SplashHandle*,Title const& title);

    static void SetProgress(SplashHandle*, scalar);

    static void Repaint(SplashHandle*);

    static bool SetBitmap(SplashHandle *s, PixelFormat, const CSize &size, const byte_t *data);

    static void DestroySplash(SplashHandle* s);
};

}
}

namespace Coffee{

using Splash = CoffeeExt::QtDialogs::QtSplash;

}