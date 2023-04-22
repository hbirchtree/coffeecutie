#pragma once

#include "blam_base_types.h"
#include "blam_tag_classes.h"
#include "blam_tag_index.h"

namespace blam::shader {
struct mod2_shader
{
    tagref_t ref;
    u32      unknown;
};

struct shader_desc
{
    tagref_t ref;
    u32      unknown[4];
};

enum class detail_map_function : u32
{
    biased_multiply,
    multiply,
    biased_add,
};
enum class animation_function : u16
{
    one,
    zero,
    cosine,
    cosine_variable,
    diagonal,
    diagonal_period,
    slide,
    slide_variable,
    noise,
    jitter,
    wander,
    spark,
};
enum class animation_src : u16
{
    none,
    A_out,
    B_out,
    C_out,
    D_out,
};
enum class reflection_map_type : u16
{
    bumped_cube,
    flat_cube,
    bumped_radiosity,
};

struct alignas(4) radiosity_properties /* aka shdr */
{
    enum class radiosity_flags : u16
    {
        simple_params   = 0x1,
        ignore_normals  = 0x2,
        transparent_lit = 0x4,
    };
    enum class radiosity_lod : u16
    {
        high,
        medium,
        low,
        turd,
    };
    enum class physics_material : u16
    {
        dirt,
        sand,
        stone,
        snow,
        wood,
        metal_hollow,
        metal_thin,
        metal_thicc,
        rubber,
        glass,
        force_field,
        grunt,
        hunter_armor,
        hunter_skin,
        elite,
        jackal,
        jackal_energy_shield,
        engineer_skin,
        engineer_force_field,
        flood_combat_form,
        flood_carrier_form,
        cyborg_armor,
        cyborg_energy_shield,
        human_armor,
        human_skin,
        sentinel,
        monitor,
        plastic,
        water,
        leaves,
        elite_energy_shield,
        ice,
        hunter_shield,
    };

    struct view_props
    {
        f32   brightness;
        Vecf3 tint_color;
    };

    radiosity_flags flags;
    radiosity_lod   detail_level;
    f32             power;
    Vecf3           emission;
    Vecf3           tint;

    u16              physics_flags;
    physics_material physics;

    template<class T>
    T const* as() const
    {
        return reinterpret_cast<T const*>(this);
    }
};

struct texture_scrolling_animation
{
    struct texture_anim_properties
    {
        animation_src      source;
        animation_function function;

        f32 period;
        f32 phase;
        f32 scale;
    };

    texture_anim_properties u;
    texture_anim_properties v;

    texture_anim_properties rot; /* rotation-animation */

    Vecf2 rotation_center;
};

struct detail_map
{
    f32                               scale;
    tagref_typed_t<tag_class_t::bitm> map;
};

struct bitm_reference_t
{
    Vecf2 uv_scale;
    Vecf2 uv_offset;
    f32   rotation;
    f32   mip_bias;

    tagref_typed_t<tag_class_t::bitm> map;
};

namespace chicago {
enum class flags_t : u8
{
    none                          = 0x0,
    alpha_testing                 = 0x1,
    decal                         = 0x2,
    two_sided                     = 0x4,
    first_map_screenspace         = 0x8,
    draw_before_water             = 0x10,
    ignore_effect                 = 0x20,
    scale_first_map_with_distance = 0x40,
    numeric                       = 0x80
};
enum class map_type : u16
{
    map_2d,
    map_cubemap,
    object_centered_cubemap,
    viewer_centered_cubemap,
};
enum class framebuffer_fade_mode : u16
{
    none,
    perpendicular,
    parallel,
};
enum class framebuffer_fade_src : u16
{
    none,
    A_out,
    B_out,
    C_out,
    D_out,
};
enum class framebuffer_blending : u16
{
    alpha_blend,
    multiply,
    double_multiply,
    add,
    subtract,
    component_min,
    component_max,
    alpha_multiply_add,
};

enum class color_function : u16
{
    current,
    next_map,
    multiply,
    double_multiply,
    add,
    add_signed_current,
    add_signed_next_map,
    subtract_current,
    subtract_next_map,
    blend_current_alpha,
    blend_current_alpha_inverse,
    blend_next_map_alpha,
    blend_next_map_alpha_inverse,
};

struct alignas(4) lens_flares_t
{
    f32                               spacing;
    tagref_typed_t<tag_class_t::lens> lens_flare;
};
enum class extra_flags : u32
{
    none                    = 0x0,
    dont_fade_active_camo   = 0x1,
    numeric_countdown_timer = 0x2,
};
enum class map_flags : u16
{
    none            = 0x0,
    unfiltered      = 0x1,
    alpha_replicate = 0x2,
    u_clamp         = 0x4,
    v_clamp         = 0x8,
};
struct map_t
{
    u32 padding_1[11];

    map_flags      flags;
    color_function color_func;
    color_function alpha_func;
    u16            padding_2;

    u32 padding_3[8];

    bitm_reference_t map;

    u32 padding_4[7];

    texture_scrolling_animation anim_2d;

    u32 padding_5[3];
};

static_assert(sizeof(map_t) == 220);

struct alignas(4) base
{
    u32                            padding__;
    u8                             numeric_counter_limit;
    chicago::flags_t               flags;
    chicago::map_type              first_map_type;
    chicago::framebuffer_blending  blend_function;
    chicago::framebuffer_fade_mode fade_mode;
    chicago::framebuffer_fade_src  fade_src;
};

} // namespace chicago

template<typename V>
struct alignas(4) shader_chicago : radiosity_properties /* aka schi */
{
    chicago::base          transparent;
    chicago::lens_flares_t lens_flares;

    reflexive_t<tagref_typed_t<tag_class_t::shdr>, V> layers;

    reflexive_t<chicago::map_t, V> maps;

    chicago::extra_flags ex_flags;
};

static_assert(offsetof(shader_chicago<pc_version_t>, lens_flares) == 52);

struct reflection_properties
{
    reflection_map_type               type;
    radiosity_properties::view_props  perpendicular;
    radiosity_properties::view_props  parallel;
    tagref_typed_t<tag_class_t::bitm> map;
    detail_map                        bump_map;
};

template<typename V>
struct alignas(4) shader_chicago_extended : radiosity_properties /* aka scex */
{
    chicago::base                                     transparent;
    chicago::lens_flares_t                            lens_flares;
    reflexive_t<tagref_typed_t<tag_class_t::shdr>, V> layers;
    reflexive_t<chicago::map_t, V>                    maps_4stage;
    reflexive_t<chicago::map_t, V>                    maps_2stage;
    chicago::extra_flags                              extra_flags;
};

static_assert(
    offsetof(shader_chicago_extended<pc_version_t>, lens_flares) == 52);

struct alignas(32) shader_glass : radiosity_properties /* aka sgla */
{
    enum class glass_flags : u32
    {
        none                      = 0x0,
        alpha_test                = 0x1,
        decal                     = 0x2,
        two_sided                 = 0x4,
        bump_map_is_specular_mask = 0x8,
    };

    glass_flags flags;

    u32 padding_[11];

    struct
    {
        Vecf3      color;
        detail_map map;

        u32 padding_[5];
    } background_tint;

    struct
    {
        reflection_map_type               type;
        view_props                        perpendicular;
        view_props                        parallel;
        tagref_typed_t<tag_class_t::bitm> map;
        detail_map                        bump_map;

        u32 padding_[33];
    } reflection;

    struct
    {
        detail_map map;
        detail_map detail;

        u32 padding_[8];
    } diffuse;

    struct
    {
        detail_map map;
        detail_map detail;
    } specular;
};

struct alignas(64) shader_meter : radiosity_properties /* aka smet, TODO */
{
    enum class meter_flags : u32
    {
        none                    = 0x0,
        decal                   = 0x1,
        two_sided               = 0x2,
        flash_color_is_negative = 0x4,
        tint_mode_2             = 0x8,
        unfiltered              = 0x10,
    };

    meter_flags flags;

    u32 padding_1[9];

    tagref_typed_t<tag_class_t::bitm> map;

    u32 padding_2[8];

    struct alignas(32)
    {
        Vecf3 gradient_min;
        Vecf3 gradient_max;
        Vecf3 background;
        Vecf3 flash;
        Vecf3 tint;
        f32   transparency;
        f32   background_transparency;
    } colors;

    struct alignas(32)
    {
        animation_src brightness;
        animation_src flash;
        animation_src value;
        animation_src gradient;
        animation_src flash_extension;
    } ext_func_src;
};

struct alignas(4) shader_water : radiosity_properties /* aka swat */
{
    enum class water_flags : u16
    {
        none                                = 0x0,
        base_map_alpha_modulates_reflect    = 0x1,
        base_map_color_modulates_background = 0x2,
        atmospheric_fog                     = 0x4,
        draw_before_fog                     = 0x8,
    };

    u32         unknown_;
    water_flags flags;
    u16         padding__;
    u32         padding_[8];

    tagref_typed_t<tag_class_t::bitm> base;

    u32 padding_2[4];

    view_props perpendicular;
    view_props parallel;

    u32 padding_3[4];

    tagref_typed_t<tag_class_t::bitm> reflection_map;

    struct ripple_t /* TODO: Find the correct layout of this structure */
    {
        f32   contribution;
        f32   anim_angle;
        f32   anim_velocity;
        Vecf2 map_offset;
        u32   map_repeats;
        u32   map_index;
    };

    u32 padding_4[4];

    struct
    {
        f32 anim_angle;
        f32 anim_velocity;
        f32 scale;

        tagref_typed_t<tag_class_t::bitm> maps;

        u32 mipmap_levels;
        f32 fade_factor;
        f32 mipmap_bias;

        u32 padding_[16];

        reflexive_t<ripple_t> ripples;
    } ripple;

    u32 padding[4];
};

static_assert(sizeof(shader_water) == 320);

struct alignas(4) shader_env : radiosity_properties /* aka senv */
{
    enum class flags_t : u16
    {
        none                      = 0x0,
        alpha_tested              = 0x1,
        bump_map_is_specular_mask = 0x2,
        true_atmospheric_fog      = 0x4,
    };
    enum class env_shader_type : u16
    {
        normal,
        blended,
        blended_base_specular,
    };
    enum class diffuse_flags : u32
    {
        none                = 0x0,
        rescale_detail_maps = 0x1,
        rescale_bump_map    = 0x2,
    };

    u32 padding_2;

    flags_t         flags;
    env_shader_type shader_type;

    chicago::lens_flares_t lens_flare;

    u32 unknown_2[17];

    struct diffuse_map_t
    {
        diffuse_flags                     flags;
        tagref_typed_t<tag_class_t::bitm> base;

        u32 padding_[6];

        detail_map_function detail_function;
        detail_map          primary;
        detail_map          secondary;

        u32 padding_2[6];

        detail_map_function micro_function;
        detail_map          micro;
        Vecf3               material_color;

        u32 padding_1[3];
    } diffuse;

    detail_map bump;

    f32 unknown_1[2];
    u32 padding_3[4];

    texture_scrolling_animation scrolling;

    /* EVERYTHING BELOW IS NOT WORKING */

    enum class illum_flags : u32
    {
        none       = 0x0,
        unfiltered = 0x1,
    };
    struct illumination_props
    {
        Vecf3              on_color;
        Vecf3              off_color;
        animation_function anim;
        f32                period;
        f32                phase;

        u32 padding_[1];
    };

    struct self_illumination_t
    {
        illum_flags flags;

        illumination_props primary;
        illumination_props secondary;
        illumination_props plasma;

        u32 padding_2[25];

        detail_map map;
    } self_illum;

    enum class specular_flags : u16
    {
        none              = 0x0,
        overbright        = 0x1,
        extra_shiny       = 0x2,
        lightmap_specular = 0x4,
    };

    u32 padding_5[9];

    struct specular_t
    {
        specular_flags flags;
        f32            brightness;
        Vecf3          perpendicular_color;
        Vecf3          parallel_color;
    } specular;

    u32 padding_6[4];

    enum class reflection_flags : u16
    {
        none           = 0x0,
        dynamic_mirror = 0x1,
    };

    struct reflection_t
    {
        reflection_flags    flags;
        reflection_map_type type;
        f32                 lightmap_brightness;

        u32 padding_1[7];

        f32 perpendicular_brightness;
        f32 parallel_brightness;

        u32 padding[10];

        tagref_typed_t<tag_class_t::bitm> reflection;
    } reflection;
};

static_assert(offsetof(shader_env, lens_flare) == 44);
static_assert(
    offsetof(shader_env, diffuse) + offsetof(shader_env::diffuse_map_t, base)
    == 136);
static_assert(offsetof(shader_env, scrolling) == 336);
static_assert(
    offsetof(shader_env, specular)
        + offsetof(shader_env::specular_t, perpendicular_color)
    == 680);
static_assert(offsetof(shader_env, reflection) == 720);
static_assert(
    offsetof(shader_env, reflection)
        + offsetof(shader_env::reflection_t, perpendicular_brightness)
    == 756);
static_assert(
    offsetof(shader_env, reflection)
        + offsetof(shader_env::reflection_t, reflection)
    == 804);

struct alignas(4) shader_model : radiosity_properties /* aka soso */
{
    enum class model_flags : u16
    {
        none                      = 0x0,
        detail_after_reflection   = 0x1,
        two_sided                 = 0x2,
        no_alpha_test             = 0x4,
        alpha_blend_decal         = 0x8,
        true_atmospheric_fog      = 0x10,
        disable_two_sided_culling = 0x20,
    };
    enum class color_src : u32
    {
        A_out,
        B_out,
        C_out,
        D_out,
    };
    enum class self_illum_flags : u32
    {
        none            = 0x0,
        no_random_phase = 0x1,
    };

    model_flags flags;
    u32         padding_1[4];
    f32         translucency;

    struct
    {
        color_src src;
    } change_color;

    u32 padding_2[23];

    struct maps_t
    {
        Vecf2 scale;

        tagref_typed_t<tag_class_t::bitm> base;
        u32                               padding_1[2];
        tagref_typed_t<tag_class_t::bitm>
            multipurpose; /* R = base color, G = illumination, B = affected by
                             change color src, A = reflection */
        u32 padding_2[2];

        struct
        {
            u16                               function;
            u16                               mask;
            f32                               scale;
            tagref_typed_t<tag_class_t::bitm> map;
            f32                               v_scale;
        } detail;
    } maps;

    u32 padding_3[19];

    struct reflection_t
    {
        f32                               falloff_distance;
        f32                               cutoff_distance;
        f32                               perpendicular_brightness;
        Vecf3                             perpendicular_tint;
        f32                               parallel_brightness;
        Vecf3                             parallel_tint;
        tagref_typed_t<tag_class_t::bitm> reflection;
    } reflection;

    u32 padding_4[17];
};

static_assert(offsetof(shader_model, translucency) == 56);
static_assert(offsetof(shader_model, maps) == 156);
static_assert(offsetof(shader_model, reflection) == 316);
static_assert(sizeof(shader_model) == 440);

struct alignas(4) shader_plasma : radiosity_properties /* aka spla */
{
    u32 padding_1[2];

    struct
    {
        animation_src source;
        f32           exponent;
    } intensity;

    struct
    {
        animation_src source;
        f32           amount;
        f32           exponent;
    } offset;

    u32 padding_2[9];

    struct
    {
        f32           perpendicular_brightness;
        Vecf3         perpendicular_tint;
        f32           parallel_brightness;
        Vecf3         parellel_tint;
        animation_src tint_src;
    } color;

    u32 padding_3[2 + 3 * 4];

    struct noise_map
    {
        f32        anim_period;
        Vecf3      anim_dir;
        detail_map noise;
    };

    noise_map primary_noise;

    noise_map secondary_noise;
};

enum class transparent_flags : u16
{
    none,
    color_mux                  = 0x1,
    alpha_mux                  = 0x2,
    a_out_controls_color0_anim = 0x4,
};

enum class color_input : u16
{
    zero,
    one,
    one_half,
    negative_one,
    negative_one_half,
    map_alpha_0,
    map_alpha_1,
    map_alpha_2,
    map_alpha_3,
    vertex_alpha_0,
    vertex_alpha_1,
    scratch_alpha_0,
    scratch_alpha_1,
    constant_alpha_0,
    constant_alpha_1,
    map_blue_0,
    map_blue_1,
    map_blue_2,
    map_blue_3,
    vertex_blue_0,
    vertex_blue_1,
    scratch_blue_0,
    scratch_blue_1,
    constant_blue_0,
    constant_blue_1,
};

enum class color_mapping : u16
{
    clamp,
    clamp_inverse,
    squared_clamp_minus_one,
    one_minus_square_clamp,
    clamp_minus_half,
    one,
    minus_one,
};

enum class color_output : u16
{
    discard,
    final_color,
    scratch_color_0 = final_color,
    scratch_color_1,
    vertex_color_0,
    vertex_color_1,
    map_color_0,
    map_color_1,
    map_color_2,
    map_color_3,
};

enum class color_output_function : u16
{
    multiply,
    dot_product,
};

enum class color_output_mapping : u16
{
    identity,
    scale_half,
    scale_2,
    scale_4,
    subtract_half, /* subtract 1/2 */
    expand_normal, /* normalize? */
};

struct transparent_stage
{
    transparent_flags flags;

    /* constants and animation */
    struct
    {
        animation_src      color0_src;
        animation_function color0_function;
        animation_src      color0_period;
        Vecf3              color0_lower_bound;
        Vecf3              color0_upper_bound;
        Vecf3              color1;
        u16                padding[2];
    } animation;

    struct inputs_t
    {
        color_input a;
        color_input a_mapping;
        color_input b;
        color_input b_mapping;
        color_input c;
        color_input c_mapping;
        color_input d;
        color_input d_mapping;
    };

    inputs_t color_inputs;
    struct
    {
        color_output          ab;
        color_output_function ab_function;
        color_output          cd;
        color_output_function cd_function;
        color_output          abcd_mux_sum;
        color_output_mapping  mapping;
        u16                   padding[2];
    } color_outputs;

    inputs_t alpha_inputs;
    struct
    {
        color_output         ab;
        color_output         cd;
        color_output         abcd_mux_sum;
        color_output_mapping mapping;
        u32                  padding[2];
    } alpha_outputs;
};

struct alignas(32) shader_transparent : radiosity_properties /* aka sotr */
{
    chicago::base          transparent;
    chicago::lens_flares_t lens_flares;

    reflexive_t<tagref_typed_t<tag_class_t::shdr>> layers;

    enum class map_flags_t
    {
        none       = 0x0,
        unfiltered = 0x1,
        u_clamped  = 0x2,
        v_clamped  = 0x4,
    };

    struct map_t
    {
        u16              flags;
        bitm_reference_t map;
        texture_scrolling_animation animation;
    };

    reflexive_t<map_t> maps;
    reflexive_t<map_t> stages;
};

static_assert(offsetof(shader_transparent, maps) == 84);
static_assert(sizeof(shader_transparent::map_t) == 100);

C_FLAGS(chicago::flags_t, u32)
C_FLAGS(shader_env::flags_t, u32)
C_FLAGS(shader_model::model_flags, u32)

} // namespace blam::shader
