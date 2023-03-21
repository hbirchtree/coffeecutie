#pragma once

#include "types.h"

#include <peripherals/error/result.h>

#include <optional>

namespace shader_proc::glsl {

using glsl_error = std::pair<std::string, std::string>;

stl_types::result<std::string, glsl_error> generate(
    spv_input&& input, target_options&& output);

std::optional<std::string> validate_source(std::string const& source);

} // namespace shader_proc::glsl
