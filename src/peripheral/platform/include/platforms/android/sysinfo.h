#pragma once

#include <peripherals/identify/system.h>
#include <peripherals/stl/types.h>

namespace platform::info::os::android {

std::optional<std::string> name();

std::optional<std::string> version();

} // namespace platform::info::os::android

namespace platform::info::device::android {

std::optional<std::pair<std::string, std::string>> device();

std::optional<std::pair<std::string, std::string>> motherboard();

std::optional<std::pair<std::string, std::string>> chassis();

} // namespace platform::info::device::android
