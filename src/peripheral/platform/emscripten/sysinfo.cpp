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
        EM_ASM_PTR({ return stringToNewUTF8(Module.uaHints.platform || navigator.platform); }));
}

/* User-Agent Client Hints high-entropy values resolve via a Promise, while
 * EM_ASM is synchronous. Kick off the request once at startup and cache the
 * result on Module.uaHints; getters below read the cache and return empty
 * until it resolves, letting callers fall back to userAgent parsing. */
static void platform_init_ua_hints()
{
#ifdef COFFEE_WASM
    EM_ASM({
        Module.uaHints = {};
        if(navigator.userAgentData)
        {
            navigator.userAgentData
                .getHighEntropyValues([
                    'architecture', 'model', 'platformVersion', 'platform'])
                .then(function(h) { Module.uaHints = h; })
                .catch(function() {});
        }
    });
#endif
}

namespace {
struct ua_hints_initializer
{
    ua_hints_initializer()
    {
        platform_init_ua_hints();
    }
} ua_hints_initializer_instance;
} // namespace

static char* platform_get_ua_arch_internal()
{
    return reinterpret_cast<char*>(EM_ASM_PTR({
        var h = Module.uaHints || {};
        if(!h.architecture)
            return stringToNewUTF8("");
        var a = h.architecture;
        if(h.bitness)
            a += "_" + h.bitness;
        return stringToNewUTF8(a);
    }));
}

static char* platform_get_ua_model_internal()
{
    return reinterpret_cast<char*>(EM_ASM_PTR({
        var h = Module.uaHints || {};
        return stringToNewUTF8(h.model || "");
    }));
}

static char* platform_get_ua_platform_version_internal()
{
    return reinterpret_cast<char*>(EM_ASM_PTR({
        var h = Module.uaHints || {};
        return stringToNewUTF8(h.platformVersion || "");
    }));
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
        {
            /* Valueless flag (?foo&bar=1): present with empty value, so
             * contains()-style checks (e.g. "dummy_plug") see it */
            if(!param.empty())
                out[std::string(param)] = {};
            continue;
        }
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

char* ua_architecture()
{
#ifdef COFFEE_WASM
    return platform_get_ua_arch_internal();
#else
    return "";
#endif
}

char* ua_model()
{
#ifdef COFFEE_WASM
    return platform_get_ua_model_internal();
#else
    return "";
#endif
}

char* ua_platform_version()
{
#ifdef COFFEE_WASM
    return platform_get_ua_platform_version_internal();
#else
    return "";
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
