#include "plat_wm.h"

#include "plat_windowtype.h"

#if defined(COFFEE_LINUX)
#include "coffee_linux/linux_wm.h"
#elif defined(COFFEE_WINDOWS)
#include "coffee_windows/win_wm.h"
#elif defined(COFFEE_APPLE)

#else
namespace Coffee{
namespace CDisplay{

struct CDNativeWindowInfo
{
};

} //CDisplay
} //Coffee

#endif

namespace Coffee{
namespace CDisplay{



}
}
