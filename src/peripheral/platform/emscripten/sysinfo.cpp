#include <platforms/emscripten/sysinfo.h>

#include <peripherals/stl/string/split.h>
#include <string>

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

EM_JS(char*, platform_get_query_string, (), {
    var lengthBytes = lengthBytesUTF8(window.location.search) + 1;
    var wasmString  = _malloc(lengthBytes);
    stringToUTF8(window.location.search, wasmString, lengthBytes + 1);
    return wasmString;
});

namespace emscripten::args {

std::map<std::string, std::string> query_params()
{
    using namespace stl_types::str::split;

    std::string query_string(platform_get_query_string());

    if(query_string.empty())
        return {};
    query_string = query_string.substr(1);

    std::map<std::string, std::string> out;
    for(auto it = spliterator<char>(query_string, '&');
        it != spliterator<char>();
        ++it)
    {
        auto param = *it;
        auto split = param.find('=');
        if(split == std::string::npos)
            continue;
        out[std::string(param.substr(0, split))] =
            std::string(param.substr(split + 1));
    }
    return out;
}

} // namespace emscripten::args

namespace platform::info::os::emscripten::detail {

char* user_agent()
{
    return platform_get_user_agent_internal();
}

char* platform()
{
    return platform_get_navigator_platform_internal();
}

} // namespace platform::info::os::emscripten::detail
