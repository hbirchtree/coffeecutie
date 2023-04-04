#pragma once

#include "types.h"

#include <peripherals/error/result.h>
#include <peripherals/typing/enum/graphics/shader_stage.h>

#include <map>

namespace shader_proc::spirv {

using spv_error = std::pair<int, std::string>;

struct shader_input
{
    typing::graphics::ShaderStage stage;
    std::string_view              content;
    std::string_view              path;
    uint32_t                      version;
    profile_t                     profile;

    std::map<std::string, std::string> defines;
};

struct output_options
{
    environment_t env{environment_t::opengl};
    uint32_t      version{450};
};

constexpr auto default_output = output_options{};

stl_types::result<spv_blob, spv_error> compile(
    shader_input&& shader, output_options const& opts = default_output);

stl_types::result<spv_blob, spv_error> link(std::vector<spv_blob>&& blobs);

} // namespace shader_proc::spirv
