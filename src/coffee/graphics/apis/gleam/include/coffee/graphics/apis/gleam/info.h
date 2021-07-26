#pragma once

#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
#include <peripherals/stl/types.h>

namespace Coffee::RHI::GLEAM {

using version_t = stl_types::Tup<i32, i32, String>;

version_t context_version();
version_t shader_version();

}
