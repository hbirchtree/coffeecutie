#pragma once

#include "data.h"
#include "selected_version.h"

#include <blam/volta/blam_scenario.h>

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <peripherals/libc/types.h>

using namespace libc_types::size_literals;

constexpr bool lowspec_hardware = compile_info::platform::is_32bit;

struct legacy_memory_budget
{
    static constexpr auto bsp_buffer         = 10_MB;
    static constexpr auto bsp_elements       = 2_MB;
    static constexpr auto mesh_buffer        = 5_MB;
    static constexpr auto mesh_elements      = 5_MB;
    static constexpr auto matrix_buffer      = 5_MB;
    static constexpr auto material_buffer    = 5_MB;
    static constexpr auto transparent_buffer = 5_MB;
    static constexpr auto bone_buffer        = 1_MB;
    static constexpr auto debug_buffer       = 5_MB;

    static constexpr auto grand_total = bsp_buffer + bsp_elements +
                                        mesh_buffer + mesh_elements +
                                        matrix_buffer + material_buffer;
};

struct modern_memory_budget
{
    static constexpr auto bsp_buffer         = 48_MB;
    static constexpr auto bsp_elements       = 16_MB;
    static constexpr auto mesh_buffer        = 48_MB;
    static constexpr auto mesh_elements      = 16_MB;
    static constexpr auto matrix_buffer      = 8_MB;
    static constexpr auto material_buffer    = 16_MB;
    static constexpr auto transparent_buffer = 16_MB;
    static constexpr auto bone_buffer        = 4_MB;
    static constexpr auto debug_buffer       = 8_MB;

    static constexpr auto grand_total = bsp_buffer + bsp_elements +
                                        mesh_buffer + mesh_elements +
                                        matrix_buffer + material_buffer;
};

struct shader_pair_t
{
    std::string_view                 vertex_file;
    std::string_view                 fragment_file;
    std::shared_ptr<gfx::program_t>& shader;
};

using memory_budget = std::
    conditional_t<lowspec_hardware, legacy_memory_budget, modern_memory_budget>;

void create_resources(compo::EntityContainer& e);
void create_shaders(compo::EntityContainer& e);
void set_resource_labels(compo::EntityContainer& e);
void create_camera(
    compo::EntityContainer&                                          e,
    const semantic::Span<const blam::scn::player_starting_location>& spawns);
void create_program(gfx::api& api, shader_pair_t&& shader_info);
