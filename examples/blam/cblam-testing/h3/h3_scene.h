#pragma once

#include "../h2/h2_scene.h"
#include "h3_lighting.h"

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

/* Halo 3's material_model options this renders; the rest are diffuse */
enum class MaterialModel : i32
{
    diffuse_only   = 0,
    two_lobe_phong = 1,
    cook_torrance  = 2,
};

/* Specular parameters, packed as the shader reads them */
struct SpecularParams
{
    /* specular coefficient, diffuse coefficient, analytical and area
     * specular contribution */
    Vecf4 coefficients{0.f, 1.f, 1.f, 1.f};
    /* two_lobe_phong: normal power, glancing power, fresnel steepness,
     * albedo tint blend. cook_torrance: roughness, albedo blend,
     * use material texture */
    Vecf4 lobe{1.f, 1.f, 5.f, 0.f};
    /* Normal tint (two_lobe_phong) or fresnel color (cook_torrance) */
    Vecf4 tint0{1.f};
    /* Glancing tint (two_lobe_phong) or specular tint (cook_torrance) */
    Vecf4 tint1{1.f};
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
    /* 0 none, 1 bump map, 2 plus detail bump; detail uv = bump uv * ratio */
    i32                                    bump_mode{0};
    MaterialModel                          model{MaterialModel::diffuse_only};
    /* 0 none, 1 base map alpha, 2 specular mask texture alpha */
    i32                                    specular_mask{0};
    SpecularParams                         specular;
    /* 0 off, 1 simple, 2 with detail, 3 three channel */
    i32                                    self_illum{0};
    Texture                                illum;
    Vecf2                                  illum_scale{1.f, 1.f};
    Texture                                illum_detail;
    Vecf2                                  illum_detail_ratio{1.f, 1.f};
    /* color (rgb) and intensity (a); three channel: a/b/c colors */
    Vecf4                                  illum_color{1.f};
    std::array<Vecf4, 3>                   illum_channels{};
    f32                                    illum_curve{2.f};
    Texture                                specular_mask_map;
    Texture                                material_map;
    Texture                                bump;
    Vecf2                                  bump_scale{1.f, 1.f};
    Texture                                bump_detail;
    Vecf2                                  bump_detail_ratio{1.f, 1.f};
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
    /* Baked light: linear SH as DXT5 slice pairs (see blam/elegy/structure.h)
     * and the max luminance of each coefficient */
    std::shared_ptr<gfx::compat::texture_2da_t> lightprobe;
    std::shared_ptr<gfx::sampler_t>             lightprobe_sampler;
    Vecf4                                       lightprobe_scale{1.f};
    f32                                         intensity_scale{1.f};
    f32                                         exposure{1.f}; /*!< Linear, at load */
    AutoExposure                                auto_exposure;
    BakedLight                                  baked;
    std::shared_ptr<gfx::buffer_t> vertex_buf;
    std::shared_ptr<gfx::buffer_t> element_buf;
};

void alloc_h3(compo::EntityContainer& e);
void alloc_h3_renderer(compo::EntityContainer& e);

} // namespace h3
