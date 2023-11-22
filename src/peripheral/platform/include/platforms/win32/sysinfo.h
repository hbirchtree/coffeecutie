#pragma once

#include "wmi.h"
#include <fmt/format.h>
#include <optional>
#include <peripherals/identify/system.h>
#include <peripherals/stl/range.h>
#include <peripherals/stl/string/trim.h>
#include <platforms/base/device_variant.h>

namespace platform::info {

namespace win32::detail {

void start_wmi();

}

namespace device::win32 {

inline std::optional<std::pair<std::string, std::string>> device()
{
    return std::pair<std::string, std::string>{
        info::wmi::query<std::string>(
            "SELECT Vendor FROM CIM_Product", L"Vendor")
            .value_or("Generic"),
        info::wmi::query<std::string>(
            "SELECT Version FROM CIM_Product", L"Version")
            .value_or("Device"),
    };
}

inline std::optional<std::string> name()
{
    auto [vendor, version] = device().value();
    return fmt::format("{} {}", vendor, version);
}

inline std::optional<std::pair<std::string, std::string>> motherboard()
{
    return std::pair<std::string, std::string>{
        info::wmi::query<std::string>(
            "SELECT Manufacturer FROM CIM_Card", L"Manufacturer")
            .value_or("Generic"),
        info::wmi::query<std::string>("SELECT Model FROM CIM_Card", L"Model")
            .value_or("Motherboard"),
    };
}

inline std::optional<std::pair<std::string, std::string>> chassis()
{
    return std::pair<std::string, std::string>{
        info::wmi::query<std::string>(
            "SELECT Manufacturer FROM CIM_PhysicalFrame", L"Manufacturer")
            .value_or("Generic"),
        info::wmi::query<std::string>(
            "SELECT Model FROM CIM_PhysicalFrame", L"Model")
            .value_or("Device"),
    };
}

inline DeviceType variant()
{
    /* TODO: Look it up in CIM_Chassis -> TypeDescriptions */
    return DeviceType::DeviceDesktop;
}

} // namespace device::win32

namespace display::win32 {

}

namespace os::win32 {

std::string architecture();

std::string kernel();

std::string kernel_version();

inline std::optional<std::string> name()
{
    return "Windows";
}

std::optional<std::string> version();

bool is_wow64();

std::optional<std::string> wine_version();

} // namespace os::win32

namespace proc::win32 {

using libc_types::u32;

inline libc_types::u32 node_count() { return 1; }
inline libc_types::u32 cpu_count() { return 1; }
inline libc_types::u32 core_count(u32 cpu = 0, u32 /*node*/ = 0) { return 1; }
inline libc_types::u32 thread_count(u32 cpu = 0, u32 /*node*/ = 0) { return 1; }

inline std::optional<std::pair<std::string, std::string>> model(
    u32 /*cpu*/ = 0, u32 /*node*/ = 0)
{
    return std::pair<std::string, std::string>{
        info::wmi::query<std::string>(
            "SELECT Manufacturer FROM CIM_Processor", L"Name")
            .value_or("Generic"),
        info::wmi::query<std::string>(
            "SELECT Model FROM CIM_Processor", L"SystemName")
            .value_or("Processor"),
    };
}

u32 frequency(bool current = false, u32 cpu = 0, u32 /*node*/ = 0);

} // namespace proc::win32

} // namespace platform::info
