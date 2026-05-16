#pragma once

#include <peripherals/base.h>

#include <peripherals/stl/types.h>
#include <platforms/base/device_variant.h>

#include <string>
#include <sys/sysctl.h>

#if defined(COFFEE_MACOS) || defined(COFFEE_IOS)

namespace platform::info {
namespace apple {
using libc_types::u64;

template<typename T>
requires std::is_same_v<T, std::string>
inline auto sysctl_by_name(const char* name)
{
    size_t len = 0;
    if(sysctlbyname(name, nullptr, &len, nullptr, 0) != 0 || len == 0)
        return std::string{};
    std::string out(len - 1, '\0');
    sysctlbyname(name, out.data(), &len, nullptr, 0);
    return out;
}

template<typename T>
requires std::is_same_v<T, u64>
inline auto sysctl_by_name(const char* name)
{
    u64    out = 0;
    size_t len = sizeof(out);
    sysctlbyname(name, &out, &len, nullptr, 0);
    return out;
}

} // namespace apple

namespace os::apple {

inline std::optional<std::string> name()
{
#if TARGET_OS_OSX
    return "macOS";
#elif TARGET_OS_IOS
    return "iOS";
#else
    return "Unknown";
#endif
}

inline std::optional<std::string> version()
{
#if TARGET_OS_OSX
    return info::apple::sysctl_by_name<std::string>("kern.osproductversion");
#else
#error apple::version not defined
#endif
}

} // namespace os::apple

namespace proc::apple {
using libc_types::u32;
using libc_types::u64;

inline std::optional<std::pair<std::string, std::string>> model(
    u32 = 0, u32 = 0)
{
#if defined(COFFEE_IOS)
    return std::pair<std::string, std::string>("Apple", "A");
#else
    /* machdep.cpu.* keys only exist on Intel Macs; Apple Silicon lacks them */
    auto vendor =
        info::apple::sysctl_by_name<std::string>("machdep.cpu.vendor");
    if(!vendor.empty())
        return std::make_pair(
            vendor,
            info::apple::sysctl_by_name<std::string>(
                "machdep.cpu.brand_string"));
    auto hw_model = info::apple::sysctl_by_name<std::string>("hw.model");
    if(!hw_model.empty())
        return std::make_pair(std::string("Apple"), hw_model);
    return std::nullopt;
#endif
}

inline u32 node_count()
{
    return 1;
}

inline u32 cpu_count()
{
    return info::apple::sysctl_by_name<u64>("hw.packages");
}

u32 core_count(u32 = 0, u32 = 0);

inline u32 thread_count(u32 = 0, u32 = 0)
{
    return info::apple::sysctl_by_name<u64>("machdep.cpu.thread_count");
}

inline u32 frequency(
    bool /*current*/ = false, u32 /*cpu*/ = 0, u32 /*node*/ = 0)
{
    return static_cast<u32>(
               info::apple::sysctl_by_name<u64>("machdep.tsc.frequency")) /
           1000000;
}

} // namespace proc::apple

namespace device::apple {

DeviceType variant();

inline std::optional<std::pair<std::string, std::string>> device()
{
    return std::make_pair(
        "Apple", info::apple::sysctl_by_name<std::string>("hw.model"));
}

} // namespace device::apple

namespace display::apple {

libc_types::f32 dpi();

}

namespace memory::apple {

inline libc_types::u64 resident()
{
    return 0u;
}

inline libc_types::u64 total()
{
    return info::apple::sysctl_by_name<libc_types::u64>("hw.memsize");
}

} // namespace memory::apple
} // namespace platform::info

#endif
