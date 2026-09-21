#pragma once

#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include "texture_format.h"

#include <gsl/span>

namespace gl::tex {

using typing::pixels::PixDesc;

/* Every format known to the registry, in a fixed order. */
gsl::span<texture_format_t const> texture_formats_view();

bool is_compiled(format_t fmt);

texture_format_t const& format_of(format_t fmt);
texture_format_t const& format_of(vk_format_t fmt);
texture_format_t const& format_of(PixDesc const& desc);

PixDesc desc_of(texture_format_t const& fmt);

} // namespace gl::tex
