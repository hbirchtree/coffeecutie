#include <coffee/core/plat/environment/emscripten/sysinfo.h>
#include <coffee/core/CDebug>

namespace Coffee{
namespace Environment{
namespace Emscripten{

CString EmSysInfo::GetSystemVersion()
{
    return cStringFormat("{0}.{1}.{2}", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);
}

}
}
}
