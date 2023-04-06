#include <platforms/emscripten/sysinfo.h>

/* Can be used for workarounds, if necessary */

EM_JS(char*, platform_get_user_agent_internal, (), {
    var lengthBytes = lengthBytesUTF8(navigator.userAgent) + 1;
    var wasmString  = _malloc(lengthBytes);
    stringToUTF8(navigator.userAgent, wasmString, lengthBytes + 1);
    return wasmString;
});

EM_JS(char*, platform_get_navigator_platform_internal, (), {
    var lengthBytes = lengthBytesUTF8(navigator.platform) + 1;
    var wasmString  = _malloc(lengthBytes);
    stringToUTF8(navigator.platform, wasmString, lengthBytes + 1);
    return wasmString;
});

namespace platform::info::os::emscripten::detail {

char* user_agent()
{
    return platform_get_user_agent_internal();
}

char* platform()
{
    return platform_get_navigator_platform_internal();
}

}
