#pragma once

#include <peripherals/libc/types.h>

#include <vulkan/vulkan_core.h>

namespace gl::tex {

enum class vk_format_t : libc_types::u32
{
#include "vk_formats.inl"
};

} // namespace gl::tex
