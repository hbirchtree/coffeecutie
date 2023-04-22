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
    sysctlbyname(name, nullptr, &len, nullptr, 0);
    std::string out(len - 1, '\0');
    sysctlbyname(name, out.data(), &len, nullptr, 0);
    return out;
}

template<typename T>
requires std::is_same_v<T, u64>
inline auto sysctl_by_name(const char* name)
{
    u64    out;
    size_t len = sizeof(out);
    sysctlbyname(name, &out, &len, nullptr, 0);
    return out;
}

} // namespace apple
namespace os::apple {

}
namespace proc::apple {
using libc_types::u32;
using libc_types::u64;

inline std::optional<std::pair<std::string, std::string>> model(
    u32 = 0, u32 = 0)
{
#if defined(COFFEE_IOS)
    return std::pair<std::string, std::string>("Apple", "A");
#else
    return std::make_pair(
        info::apple::sysctl_by_name<std::string>("machdep.cpu.vendor"),
        info::apple::sysctl_by_name<std::string>("machdep.cpu.brand_string"));
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
        info::apple::sysctl_by_name<u64>("machdep.tsc.frequency"));
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
} // namespace platform::info

#endif
