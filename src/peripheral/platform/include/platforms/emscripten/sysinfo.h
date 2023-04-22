#pragma once

#include <peripherals/identify/architecture.h>

#include <emscripten.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/stl/types.h>
#include <string_view>

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

} // namespace detail

inline constexpr std::string_view kernel()
{
    using namespace std::literals;
    return "Emscripten"sv;
}

inline constexpr std::string_view kernel_version()
{
    using namespace std::literals;
    return C_STR(__EMSCRIPTEN_major__) "." C_STR(
        __EMSCRIPTEN_minor__) "." C_STR(__EMSCRIPTEN_tiny__) ""sv;
}

inline std::string architecture()
{
    if(auto tmp = detail::platform())
    {
        std::string out = tmp;
        ::free(tmp);
        return out;
    }
    return COFFEE_ARCH;
}

inline std::optional<std::string> name()
{
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
    return std::nullopt;
}

inline std::optional<std::string_view> version()
{
    using namespace std::string_view_literals;
    return ""sv;
}

} // namespace platform::info::os::emscripten

namespace platform::info::device::emscripten {

inline std::optional<std::pair<std::string, std::string>> device()
{
    if(auto plat = os::emscripten::detail::platform())
    {
        std::string out = plat;
        ::free(plat);
        return std::pair<std::string, std::string>{"<unknown>", out};
    }
    return std::nullopt;
}

inline libc_types::f32 dpi()
{
    return static_cast<libc_types::f32>(
        EM_ASM_DOUBLE({ return window['devicePixelRatio']; }));
}

} // namespace platform::info::device::emscripten
