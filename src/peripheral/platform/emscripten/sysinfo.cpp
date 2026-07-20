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

namespace {

/* window.location.search returns the query string exactly as it appears
 * in the URL (browsers never decode it for you, unlike URLSearchParams) --
 * so a value built with encodeURIComponent() (eg. a "ws://host:port" server
 * URL, which needs ':' and '/' escaped to safely ride inside a query
 * value) arrives here still percent-encoded. Decode both keys and values
 * so callers get the same string they'd get from URLSearchParams. */
std::string percent_decode(std::string_view in)
{
    std::string out;
    out.reserve(in.size());
    for(size_t i = 0; i < in.size(); ++i)
    {
        if(in[i] == '%' && i + 2 < in.size())
        {
            auto hex_val = [](char c) -> int {
                if(c >= '0' && c <= '9')
                    return c - '0';
                if(c >= 'a' && c <= 'f')
                    return c - 'a' + 10;
                if(c >= 'A' && c <= 'F')
                    return c - 'A' + 10;
                return -1;
            };
            int hi = hex_val(in[i + 1]);
            int lo = hex_val(in[i + 2]);
            if(hi >= 0 && lo >= 0)
            {
                out.push_back(static_cast<char>((hi << 4) | lo));
                i += 2;
                continue;
            }
        }
        out.push_back(in[i]);
    }
    return out;
}

} // namespace

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
                out[percent_decode(param)] = {};
            continue;
        }
        out[percent_decode(param.substr(0, split))] =
            percent_decode(param.substr(split + 1));
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
