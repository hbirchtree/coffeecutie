#include <coffee/core/plat/file/osx/file.h>

#include <coffee/core/plat/plat_environment.h>
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFString.h>

namespace Coffee{
namespace CResources{
namespace Mac{

#if defined(COFFEE_APPLE_MOBILE)
static CString GetApplicationHome()
{
    char* home = getenv("HOME");
    if(!home)
        RUNOUTTHEWINDOW();
    return home;
}
#endif

}
}
}
