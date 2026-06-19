#pragma once

#include <emscripten.h>
#include <peripherals/identify/architecture.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/string/split.h>
#include <peripherals/stl/string/trim.h>
#include <peripherals/stl/types.h>
#include <platforms/base/device_variant.h>
#include <string_view>

namespace emscripten::args {

std::map<std::string, std::string> query_params();

}

namespace platform::info::memory::emscripten {

inline libc_types::u64 resident()
{
    return 0u;
}

inline libc_types::u64 total()
{
    return static_cast<libc_types::u64>(
               EM_ASM_INT({ return navigator['deviceMemory']; })) *
           1024 * 1024 * 1024;
}

} // namespace platform::info::memory::emscripten

namespace platform::info::proc::emscripten {

inline libc_types::u32 node_count()
{
    return 1;
}

inline libc_types::u32 cpu_count()
{
    return 1;
}

inline libc_types::u32 core_count(libc_types::u32 = 0, libc_types::u32 = 0)
{
    return static_cast<libc_types::u32>(
        EM_ASM_INT({ return navigator['hardwareConcurrency']; }));
}

inline libc_types::u32 thread_count(libc_types::u32 = 0, libc_types::u32 = 0)
{
    return core_count();
}

} // namespace platform::info::proc::emscripten

namespace platform::info::os::emscripten {
namespace detail {

extern char* user_agent();
extern char* platform();
extern char* ua_architecture();
extern char* ua_model();
extern char* ua_platform_version();
extern bool  is_mobile();

} // namespace detail

inline constexpr std::string_view kernel()
{
    using namespace std::literals;
    return "Emscripten"sv;
}

inline constexpr std::string_view kernel_version()
{
    using namespace std::literals;
    return C_STR(__EMSCRIPTEN_MAJOR__) "." C_STR(
        __EMSCRIPTEN_MINOR__) "." C_STR(__EMSCRIPTEN_TINY__) ""sv;
}

inline std::string architecture()
{
#ifdef COFFEE_WASM
    // Prefer Client Hints arch (e.g. "x86_64"); falls back when unresolved
    if(auto arch = detail::ua_architecture())
    {
        std::string out = arch;
        ::free(arch);
        if(!out.empty())
            return out;
    }
    if(auto tmp = detail::platform())
    {
        std::string out = tmp;
        ::free(tmp);
        return out;
    }
#endif
    return COFFEE_ARCH;
}

inline std::optional<std::string> name()
{
#ifdef COFFEE_WASM
    if(auto platform = os::emscripten::detail::platform())
    {
        std::string out(platform);
        ::free(platform);
        return out;
    }
    if(auto ua_ = os::emscripten::detail::user_agent())
    {
        std::string_view ua       = ua_;
        auto             start_id = ua.find('(');
        auto             end_id   = ua.find(')');
        if(start_id != std::string::npos && end_id != std::string::npos)
        {
            ua = ua.substr(start_id, end_id - start_id);
            std::string_view last;
            for(auto id : stl_types::str::split::str(ua, ';'))
            {
                if(!last.empty())
                {
                    id       = stl_types::str::trim::left(id);
                    auto out = std::string(id.begin(), id.end());
                    ::free(ua_);
                    return out;
                }
                last = id;
            }
        }
        ::free(ua_);
    }
#endif
    return std::nullopt;
}

inline std::optional<std::string> browser_name()
{
#ifdef COFFEE_WASM
    if(auto ua_ = os::emscripten::detail::user_agent())
    {
        std::string ua(ua_);
        ::free(ua_);
        return ua;
    }
#endif
    return std::nullopt;
}

inline std::optional<std::string> version()
{
#ifdef COFFEE_WASM
    // Client Hints platformVersion (e.g. "14.0.0"); empty until resolved
    if(auto v = detail::ua_platform_version())
    {
        std::string out = v;
        ::free(v);
        if(!out.empty())
            return out;
    }
#endif
    return std::nullopt;
}

} // namespace platform::info::os::emscripten

namespace platform::info::device::emscripten {

inline std::optional<std::pair<std::string, std::string>> device()
{
#ifdef COFFEE_WASM
    std::string model;
    if(auto m = os::emscripten::detail::ua_model())
    {
        model = m;
        ::free(m);
    }
    if(auto plat = os::emscripten::detail::platform())
    {
        std::string out = plat;
        ::free(plat);
        return std::pair<std::string, std::string>{
            out, model.empty() ? "<unknown>" : model};
    }
#endif
    return std::nullopt;
}

inline libc_types::f32 dpi()
{
    return 1.f;
    // return emscripten_get_device_pixel_ratio();
}

inline DeviceType variant()
{
    if(info::os::emscripten::detail::is_mobile())
        return DeviceType::DevicePhone;
    else
        return DeviceType::DeviceDesktop;
}

} // namespace platform::info::device::emscripten
