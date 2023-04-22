#pragma once

#include <optional>

#include "types.h"

#include <peripherals/error/result.h>

namespace shader_proc::opt {

enum class optimization_level
{
    none,
    size,
    fast,
};

struct optimizations
{
    std::optional<std::string_view> rename_entrypoint;
    optimization_level              opt_level{optimization_level::fast};
};

stl_types::result<spv_blob, std::string> perform_optimization(
    spv_input const&      input,
    target_options const& out_options,
    optimizations const&  optimizations = {});

} // namespace shader_proc::opt
