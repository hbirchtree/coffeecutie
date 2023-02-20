#pragma once

#include "data.h"
#include "selected_version.h"

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <peripherals/libc/types.h>

using namespace libc_types::size_literals;

struct legacy_memory_budget
{
    static constexpr auto bsp_buffer      = 10_MB;
    static constexpr auto bsp_elements    = 2_MB;
    static constexpr auto mesh_buffer     = 5_MB;
    static constexpr auto mesh_elements   = 5_MB;
    static constexpr auto matrix_buffer   = 5_MB;
    static constexpr auto material_buffer = 5_MB;
    static constexpr auto debug_buffer    = 5_MB;

    static constexpr auto grand_total = bsp_buffer + bsp_elements + mesh_buffer
                                        + mesh_elements + matrix_buffer
                                        + material_buffer;
};

struct modern_memory_budget
{
    static constexpr auto bsp_buffer      = 30_MB;
    static constexpr auto bsp_elements    = 15_MB;
    static constexpr auto mesh_buffer     = 35_MB;
    static constexpr auto mesh_elements   = 15_MB;
    static constexpr auto matrix_buffer   = 16_MB;
    static constexpr auto material_buffer = 16_MB;
    static constexpr auto debug_buffer    = 5_MB;

    static constexpr auto grand_total = bsp_buffer * 2 + bsp_elements
                                        + mesh_buffer + mesh_elements
                                        + matrix_buffer + material_buffer;
};

using memory_budget = std::conditional_t<
    compile_info::platform::is_32bit,
    legacy_memory_budget,
    modern_memory_budget>;

void create_resources(compo::EntityContainer& e);
void create_shaders(compo::EntityContainer& e);
void create_camera(
    compo::EntityContainer&                              e,
    const semantic::Span<const blam::scn::player_spawn>& spawns);
