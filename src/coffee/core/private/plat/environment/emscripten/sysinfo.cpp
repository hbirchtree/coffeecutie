#include <coffee/core/plat/environment/emscripten/sysinfo.h>
#include <coffee/core/CDebug>

#include <emscripten.h>

/* Can be used for workarounds, if necessary */
EM_JS(char*, get_user_agent, (), {
    var lengthBytes = lengthBytesUTF8(navigator.userAgent) + 1;
    var wasmString = _malloc(lengthBytes);
    stringToUTF8(navigator.userAgent, wasmString, lengthBytes + 1);
    return wasmString;
});

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
