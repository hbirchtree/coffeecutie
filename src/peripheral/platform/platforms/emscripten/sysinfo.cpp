#include <platforms/emscripten/sysinfo.h>

#include <peripherals/stl/types.h>

#include <emscripten.h>

/* Can be used for workarounds, if necessary */
EM_JS(char*, get_user_agent, (), {
    var lengthBytes = lengthBytesUTF8(navigator.userAgent) + 1;
    var wasmString  = _malloc(lengthBytes);
    stringToUTF8(navigator.userAgent, wasmString, lengthBytes + 1);
    return wasmString;
});

namespace platform {
namespace env {
namespace emscripten {

CString SysInfo::GetSystemVersion()
{
    return C_STR(__EMSCRIPTEN_major__) "." C_STR(
        __EMSCRIPTEN_minor__) "." C_STR(__EMSCRIPTEN_tiny__);
}

} // namespace emscripten
} // namespace env
} // namespace platform
