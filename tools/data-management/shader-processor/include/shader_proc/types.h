#pragma once

#include <cstdint>
#include <map>
#include <string_view>
#include <vector>

#include <peripherals/typing/enum/graphics/shader_stage.h>

namespace shader_proc {

using namespace std::string_view_literals;

using spv_blob = std::vector<uint32_t>;

enum class environment_t
{
    opengl,
};

enum class profile_t
{
    none,
    core,
    es,
};

struct spv_input
{
    spv_blob                      content;
    typing::graphics::ShaderStage stage;
};

struct target_options
{
    profile_t profile;
    uint32_t  version;
    std::string_view                        entrypoint{"main"sv};
    std::map<std::string_view, std::string> specializations{};
    bool strip_debug{false};
};

} // namespace shader_proc
