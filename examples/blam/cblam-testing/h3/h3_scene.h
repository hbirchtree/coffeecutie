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
    detail_overlay, /*!< two_detail_overlay */
    three_detail,   /*!< three_detail_blend */
    water,          /*!< rmw, drawn after a copy of the scene */
    count,
};

inline constexpr std::string_view variant_files[] = {
    "h3_base",
    "h3_detail",
    "h3_two_detail",
    "h3_detail_blend",
    "h3_terrain",
    "h3_detail_overlay",
    "h3_three_detail",
    "h3_water",
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
    none           = 3, /*!< Halograms: no lighting, self-illumination only */
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

struct CubeTexture
{
    std::shared_ptr<gfx::texture_cube_t> texture;
    std::shared_ptr<gfx::sampler_t>      sampler;
    f32                                  curve{1.f}; /*!< Decode exponent */
};

/* A cubemap point of the BSP and its image */
struct CubemapProbe
{
    Vecf3 position;
    u32   cubemap;
};

/* shader_water's parameters, as water_shading.fx reads them */
struct WaterParams
{
    Texture                                   color;      /*!< watercolor_texture */
    CubeTexture                               reflection; /*!< environment_map */
    std::shared_ptr<gfx::compat::texture_2da_t> slopes;   /*!< wave_slope_array */
    std::shared_ptr<gfx::sampler_t>           slopes_sampler;
    Texture                                   foam;
    Texture                                   foam_detail;
    Texture                                   shape;      /*!< global_shape_texture: painted foam in b */
    std::shared_ptr<gfx::compat::texture_2da_t> displacement; /*!< wave_displacement_array */
    std::shared_ptr<gfx::sampler_t>           displacement_sampler;
    Vecf4 slope_xform{1.f, 1.f, 0.f, 0.f};  /*!< wave_slope_array uv scale/offset */
    Vecf4 foam_detail_xform{1.f, 1.f, 0.f, 0.f};
    /* foam height, foam pow, 1 auto / 2 paint / 3 both, displacement frames */
    Vecf4 foam_params{0.5f, 2.f, 0.f, 0.f};
    Vecf4 displacement_range{0.3f, 0.2f, 0.3f, 0.f};
    Vecf4 shape_xform{1.f, 1.f, 0.f, 0.f};  /*!< wave_displacement_array uv scale/offset */
    Vecf4 foam_xform{1.f, 1.f, 0.f, 0.f};
    /* slope range xy, detail slope scale xy */
    Vecf4 slope{0.5f, 0.4f, 10.f, 3.f};
    /* wave height, aux height, detail steepness, minimal disturbance */
    Vecf4 heights{0.5f, 0.6f, 0.5f, 1.f};
    /* watercolor coefficient, reflection coefficient, fresnel r0, murkiness */
    Vecf4 shading{1.f, 1.f, 0.1f, 3.f};
    /* refraction shift, extinct distance, depth dominant ratio, bank depth */
    Vecf4 refraction{0.12f, 1000.f, 0.95f, 0.22f};
    /* sunspot cut, shadow intensity mark, slope array layers, has reflection */
    Vecf4 misc{0.01f, 0.5f, 1.f, 0.f};
    Vecf4 diffuse{0.f};                     /*!< water_diffuse */
    /* Decode exponents: watercolor, reflection */
    Vecf2 curves{2.f, 2.f};
};

/* One render method: its textures and every draw range using it */
/*! An object's draws in object space, placed by its entity's Model */
struct ObjectDraws
{
    u64                                    entity{0};
    std::vector<gfx::draw_command::data_t> draws;
};

struct Material
{
    std::vector<gfx::draw_command::data_t> draws;
    std::vector<ObjectDraws>               object_draws;
    std::string                            name;
    Variant                                variant{Variant::base};
    Texture                                diffuse;
    Vecf2                                  uv_scale{1.f, 1.f};
    Texture                                detail;
    Vecf2                                  detail_scale{1.f, 1.f};
    Texture                                detail2;
    Vecf2                                  detail2_scale{1.f, 1.f};
    /* detail_map_overlay or detail_map3, at base uv * ratio */
    Texture                                detail3;
    Vecf2                                  detail3_ratio{1.f, 1.f};
    f32                                    detail3_curve{2.f};
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
    bool                                   black_point{false}; /*!< two_detail_black_point */
    Texture                                illum;
    Vecf2                                  illum_scale{1.f, 1.f};
    Texture                                illum_detail;
    Vecf2                                  illum_detail_ratio{1.f, 1.f};
    /* color (rgb) and intensity (a); three channel: a/b/c colors */
    Vecf4                                  illum_color{1.f};
    std::array<Vecf4, 3>                   illum_channels{};
    f32                                    illum_curve{2.f};
    /* multilayer: depth, contrast, layer count, darkening; uv scale and
     * aspect ratio */
    Vecf4                                  illum_layers{0.f, 1.f, 4.f, 1.f};
    Vecf4                                  illum_layer_xform{1.f, 1.f, 1.f, 0.f};
    Texture                                specular_mask_map;
    /* environment_mapping: 0 none, 1 dynamic (the BSP's cubemaps), 2
     * per_pixel (its own); tint rgb, contribution a */
    i32                                    env_mode{0};
    CubeTexture                            environment_map;
    Vecf4                                  env_tint{1.f, 1.f, 1.f, 0.f};
    Texture                                material_map;
    Texture                                bump;
    Vecf2                                  bump_scale{1.f, 1.f};
    Texture                                bump_detail;
    Vecf2                                  bump_detail_ratio{1.f, 1.f};
    std::array<TerrainLayer, 4>            layers;
    Texture                                blend;
    Vecf2                                  blend_scale{1.f, 1.f};
    bool                                   alpha_test{false};
    Texture                                alpha_test_map; /*!< Tested instead of the base alpha */
    Vecf2                                  alpha_test_ratio{1.f, 1.f};
    bool                                   additive{false};
    bool                                   blended{false}; /*!< alpha blend */
    bool                                   constant_color{false};
    Vecf4                                  tint{1.f, 1.f, 1.f, 1.f};
    WaterParams                            water;
    bool                                   visible{true};
};

/*! A light attached to a placed object, as simple_lights.fx takes it: in
 * the object's space when it moves (entity), world space otherwise */
struct DynamicLight
{
    u64   entity{0};
    Vecf3 position{};
    Vecf3 direction{0.f, 0.f, -1.f}; /*!< Towards where it shines */
    Vecf3 color{};                   /*!< Times intensity */
    f32   size{1.f};                 /*!< Softens the distance falloff */
    f32   sphere{1.f};               /*!< Share of light outside the cone */
    f32   smooth{1.f};
    Vecf4 falloff{1.f, 0.f, 0.f, 1.f}; /*!< Distance and angle: scale xy, offset zw */
    f32   radius{0.f};
};

/* Structure BSPs one zone set can load at once, each with its lightmap */
inline constexpr size_t max_bsp_slots = 8;

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
    /* Per loaded BSP: SH coefficient scales, intensity scales four a vec4 */
    std::array<Vecf4, max_bsp_slots>            lightprobe_scales{};
    std::array<Vecf4, max_bsp_slots / 4>        intensity_scales{};
    f32                                         exposure{1.f}; /*!< Linear, at load */
    AutoExposure                                auto_exposure;
    /* Halo 3's v_atmosphere_constant_0..5 and _extra for compute_scattering
     * (atmosphere.fx); disabled when constant 1's w is negative */
    std::array<Vecf4, 7>                        atmosphere{Vecf4(0.f), Vecf4(0.f, 0.f, 0.f, -1.f)};
    std::vector<BakedLight>                     baked; /*!< Per loaded BSP */
    std::vector<CubeTexture>                    cubemaps;
    std::vector<CubemapProbe>                   cubemap_probes;
    std::vector<DynamicLight>                   lights;

    /*! The baked light under `position`, from whichever loaded BSP's
     * lightmap lies highest below it */
    std::optional<el::vertex_light_t> light_below(Vecf3 const& position) const
    {
        std::optional<el::vertex_light_t> out;
        f32                               best = -std::numeric_limits<f32>::max();
        for(auto const& bsp : baked)
        {
            f32 height = 0.f;
            if(auto light = bsp.below(position, &height); light && height > best)
                out = light, best = height;
        }
        return out;
    }
    std::shared_ptr<gfx::buffer_t> vertex_buf;
    std::shared_ptr<gfx::buffer_t> element_buf;
};

void alloc_h3(compo::EntityContainer& e);
void alloc_h3_renderer(compo::EntityContainer& e);

} // namespace h3
