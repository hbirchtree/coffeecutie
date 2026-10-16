#pragma once

#include "../h2/h2_scene.h"

#include <coffee/components/subsystem.h>
#include <coffee/graphics/apis/gleam/rhi_buffer.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

/* The Halo 3 world as H3MapLoader builds it for H3Renderer. Shares the
 * Halo 2 vertex layout and programs for now */
namespace h3 {

using h2::Texture;
using h2::Vertex;

/* Fragment program per material family; see fragments/h3_common.glsl */
enum class Variant : u8
{
    base,
    detail,
    two_detail,
    detail_blend,
    terrain,
    count,
};

inline constexpr std::string_view variant_files[] = {
    "h3_base",
    "h3_detail",
    "h3_two_detail",
    "h3_detail_blend",
    "h3_terrain",
};

static_assert(std::size(variant_files) == static_cast<size_t>(Variant::count));

/* One layer of a terrain material */
struct TerrainLayer
{
    Texture base;
    Texture detail;
    Vecf4   scale{1.f, 1.f, 1.f, 1.f}; /*!< base uv scale, detail uv scale */
    f32     base_curve{2.f};
    f32     detail_curve{2.f};
    bool    enabled{false};
    bool    detailed{false};
};

/* One render method: its textures and every draw range using it */
struct Material
{
    std::vector<gfx::draw_command::data_t> draws;
    std::string                            name;
    Variant                                variant{Variant::base};
    Texture                                diffuse;
    Vecf2                                  uv_scale{1.f, 1.f};
    Texture                                detail;
    Vecf2                                  detail_scale{1.f, 1.f};
    Texture                                detail2;
    Vecf2                                  detail2_scale{1.f, 1.f};
    /* Decode exponents of base, detail, detail2 (see h3_common.glsl) */
    Vecf3                                  curves{2.f, 2.f, 2.f};
    std::array<TerrainLayer, 4>            layers;
    Texture                                blend;
    Vecf2                                  blend_scale{1.f, 1.f};
    bool                                   alpha_test{false};
    bool                                   additive{false};
    bool                                   blended{false}; /*!< alpha blend */
    bool                                   constant_color{false};
    Vecf4                                  tint{1.f, 1.f, 1.f, 1.f};
    bool                                   visible{true};
};

struct H3Scene : compo::SubsystemBase
{
    using type = H3Scene;

    bool loaded{false};
    u32  generation{0}; /*!< Bumped per load, so readers can resync */

    std::vector<Material>          materials;
    std::shared_ptr<gfx::buffer_t> vertex_buf;
    std::shared_ptr<gfx::buffer_t> element_buf;
};

void alloc_h3(compo::EntityContainer& e);
void alloc_h3_renderer(compo::EntityContainer& e);

} // namespace h3
