#include <coffee/windowing/windowing/ndkwindow/ndk_window.h>

#if defined(COFFEE_USE_ANDROID_NATIVEWIN)

#include <coffee/android/android_main.h>

namespace Coffee{
namespace Display{

CSize NDKWindow::windowSize() const
{
    int winSize[2] = {};

    CoffeeForeignSignalHandleNA(CoffeeForeign_GetWinSize,
                                winSize, nullptr, nullptr);

    return {winSize[0], winSize[1]};
}

}
}
#endif
