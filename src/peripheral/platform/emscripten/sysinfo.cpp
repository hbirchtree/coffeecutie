#include <platforms/emscripten/sysinfo.h>

#include <peripherals/stl/string/split.h>
#include <string>

/* Can be used for workarounds, if necessary */

EM_JS_DEPS(coffee_sysinfo_js_deps, "$stringToNewUTF8");

static char* platform_get_user_agent_internal()
{
    return reinterpret_cast<char*>(
        EM_ASM_PTR({ return stringToNewUTF8(navigator.userAgent); }));
}

static char* platform_get_navigator_platform_internal()
{
    return reinterpret_cast<char*>(
        EM_ASM_PTR({ return stringToNewUTF8(navigator.platform); }));
}

static char* platform_get_query_string()
{
    return reinterpret_cast<char*>(
        EM_ASM_PTR({ return stringToNewUTF8(window.location.search); }));
}

namespace emscripten::args {

std::map<std::string, std::string> query_params()
{
#ifdef COFFEE_WASM
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
#else
    return {};
#endif
}

} // namespace emscripten::args

namespace platform::info::os::emscripten::detail {

char* user_agent()
{
#ifdef COFFEE_WASM
    return platform_get_user_agent_internal();
#else
    return "Mozilla/5.0";
#endif
}

char* platform()
{
#ifdef COFFEE_WASM
    return platform_get_navigator_platform_internal();
#else
    return "Linux x86_64";
#endif
}

bool is_mobile()
{
#ifdef COFFEE_WASM
    return EM_ASM_INT({
        return navigator.userAgentData && navigator.userAgentData.mobile ? 1
                                                                         : 0;
    });
#else
    return false;
#endif
}

} // namespace platform::info::os::emscripten::detail
