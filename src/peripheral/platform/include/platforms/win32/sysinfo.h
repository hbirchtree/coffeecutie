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
            L"SELECT Vendor FROM CIM_Product", L"Vendor")
            .value_or("Generic"),
        info::wmi::query<std::string>(
            L"SELECT Version FROM CIM_Product", L"Version")
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
            L"SELECT Manufacturer FROM Win32_BaseBoard", L"Manufacturer")
            .value_or("Generic"),
        info::wmi::query<std::string>(
            L"SELECT Product FROM Win32_BaseBoard", L"Product")
            .value_or("Motherboard"),
    };
}

inline std::optional<std::pair<std::string, std::string>> chassis()
{
    return std::pair<std::string, std::string>{
        info::wmi::query<std::string>(
            L"SELECT Manufacturer FROM CIM_PhysicalFrame", L"Manufacturer")
            .value_or("Generic"),
        info::wmi::query<std::string>(
            L"SELECT Product FROM CIM_PhysicalFrame", L"Product")
            .value_or("Device"),
    };
}

inline DeviceType variant()
{
    /* TODO: Look it up in CIM_Chassis -> TypeDescriptions */
    auto type =
        info::wmi::query<libc_types::u32>(
            L"SELECT PCSystemType FROM Win32_ComputerSystem", L"PCSystemType")
            .value_or(1);
    switch(type)
    {
    case 2:
    case 6:
        return DeviceType::DeviceLaptop;
    default:
        return DeviceType::DeviceDesktop;
    }
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

std::optional<std::pair<std::string, std::string>> wine_host_system();

} // namespace os::win32

namespace proc::win32 {

using libc_types::u32;

inline libc_types::u32 node_count()
{
    return 1;
}

inline libc_types::u32 cpu_count()
{
    return info::wmi::query<u32>(
               L"SELECT NumberOfProcessors FROM Win32_ComputerSystem",
               L"NumberOfProcessors")
        .value_or(1);
}

inline libc_types::u32 core_count(u32 cpu = 0, u32 /*node*/ = 0)
{
    return info::wmi::query<u32>(
               L"SELECT NumberOfCores FROM Win32_Processor", L"NumberOfCores")
        .value_or(1);
}

inline libc_types::u32 thread_count(u32 cpu = 0, u32 /*node*/ = 0)
{
    return info::wmi::query<u32>(
               L"SELECT NumberOfLogicalProcessors FROM Win32_Processor",
               L"NumberOfLogicalProcessors")
        .value_or(1);
}

inline std::optional<std::pair<std::string, std::string>> model(
    u32 /*cpu*/ = 0, u32 /*node*/ = 0)
{
    return std::pair<std::string, std::string>{
        info::wmi::query<std::string>(
            L"SELECT Manufacturer FROM Win32_Processor", L"Manufacturer")
            .value_or("Generic"),
        info::wmi::query<std::string>(
            L"SELECT Name FROM Win32_Processor", L"Name")
            .value_or("Processor"),
    };
}

inline u32 frequency(bool current = false, u32 cpu = 0, u32 /*node*/ = 0)
{
    return info::wmi::query<u32>(
               L"SELECT MaxClockSpeed FROM Win32_Processor", L"MaxClockSpeed")
               .value_or(0) *
           1000;
}

} // namespace proc::win32

namespace memory::win32 {

inline libc_types::u64 total()
{
    return info::wmi::query<libc_types::u64>(
               L"SELECT TotalPhysicalMemory FROM Win32_ComputerSystem",
               L"TotalPhysicalMemory")
               .value_or(0u);
}

inline libc_types::u64 resident()
{
    return 0;
}

} // namespace memory::win32

} // namespace platform::info
