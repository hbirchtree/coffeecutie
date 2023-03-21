#pragma once

#include <spirv-tools/optimizer.hpp>

namespace shader_proc::opt::passes {

struct compatibility_options
{
    bool ssbo_as_ubo{false};
    bool ubo_as_uniforms{false};
    bool remove_flat_qualifier{false};
    bool unsigned_to_signed_int{false};
    bool float64_to_float32{false};
    bool int_stage_input_to_float{false};
};

spvtools::Optimizer::PassToken CreateLegacyCompatibilityPass(
    compatibility_options&& options = {});

spvtools::Optimizer::PassToken CreateRenameEntrypointPass(
    std::string_view new_name);

} // namespace shader_proc::opt::passes
