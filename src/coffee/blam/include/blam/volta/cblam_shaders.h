#pragma once

#include "cblam_base_types.h"
#include "cblam_tag_index.h"

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

struct alignas(4) shader_base /* aka shdr */
{
    enum class radiosity_flags : u32
    {
        simple_params   = 0x1,
        ignore_normals  = 0x2,
        transparent_lit = 0x4,
    };
    enum class radiosity_lod : u32
    {
        high,
        medium,
        low,
        turd,
    };
    enum class physics_material : u32
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
    enum class detail_map_function : u32
    {
        biased_multiply,
        multiply,
        biased_add,
    };
    enum class animation_function : u16
    {
        none,
    };
    enum class animation_src : u16
    {
        none,
        A_out,
        B_out,
        C_out,
        D_out,
    };
    enum class reflection_map_type : u32
    {
        none,
        bumped_cube,
        flat_cube,
        bumped_radiosity,
    };

    struct detail_map
    {
        scalar                            scale;
        tagref_typed_t<tag_class_t::bitm> map;
    };

    struct view_props
    {
        scalar brightness;
        Vecf3  tint_color;
    };

    radiosity_flags  flags;
    radiosity_lod    detail_level;
    Vecf3            emission;
    Vecf3            tint;
    physics_material physics;
};

struct texture_scrolling_animation
{
    struct texture_anim_properties
    {
        shader_base::animation_src      source;
        shader_base::animation_function function;

        scalar period;
        scalar phase;
        scalar repeats;
    };

    texture_anim_properties u;
    texture_anim_properties v;

    texture_anim_properties rot; /* rotation-animation */

    Vecf2 rotation_center;
};

namespace chicago {
enum class flags : u16
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

struct lens_flares_t
{
    scalar                            spacing;
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

    map_flags flags;
    u16       color_function;
    u16       alpha_function;
    u16       padding_2;

    u32 padding_3[8];

    struct
    {
        Vecf2  uv_scale;
        Vecf2  uv_offset;
        scalar rotation;
        scalar mip_bias;

        tagref_typed_t<tag_class_t::bitm> map;
    } map;

    u32 padding_4[8];

    texture_scrolling_animation anim_2d;

    u32 padding_5[2];
};

C_FLAGS(flags, u32)

} // namespace chicago

template<typename V>
struct alignas(4) shader_chicago : shader_base /* aka schi */
{
    u8                             numeric_counter_limit;
    chicago::flags                 flags;
    chicago::map_type              first_map_type;
    chicago::framebuffer_blending  blend_function;
    chicago::framebuffer_fade_mode fade_mode;
    chicago::framebuffer_fade_src  fade_src;

    u32 padding_1;

    chicago::lens_flares_t lens_flares;

    reflexive_t<tagref_typed_t<tag_class_t::shdr>, V> layers;

    reflexive_t<chicago::map_t, V> maps;

    chicago::extra_flags ex_flags;
};

struct reflection_properties
{
    shader_base::reflection_map_type  type;
    shader_base::view_props           perpendicular;
    shader_base::view_props           parallel;
    tagref_typed_t<tag_class_t::bitm> map;
    shader_base::detail_map           bump_map;
};

template<typename V>
struct alignas(4) shader_chicago_extended : shader_base /* aka scex */
{
    u8                counter_limit;
    u8                padding_1;
    chicago::flags    flags;
    chicago::map_type first_map_type;

    chicago::framebuffer_blending  blend_mode;
    chicago::framebuffer_fade_mode fade_mode;
    chicago::framebuffer_fade_src  fade_src;

    u32 padding_2;

    chicago::lens_flares_t lens_flares;

    u32 padding_3[3];

    reflexive_t<tagref_typed_t<tag_class_t::shdr>, V> layers;

    reflexive_t<chicago::map_t, V> maps_4stage;
    reflexive_t<chicago::map_t, V> maps_2stage;
    chicago::extra_flags           extra_flags;
};

struct alignas(4) shader_glass : shader_base /* aka sgla */
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

struct alignas(64) shader_meter : shader_base /* aka smet, TODO */
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
        Vecf3  gradient_min;
        Vecf3  gradient_max;
        Vecf3  background;
        Vecf3  flash;
        Vecf3  tint;
        scalar transparency;
        scalar background_transparency;
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

template<typename V>
struct alignas(4) shader_water : shader_base /* aka swat */
{
    enum class water_flags : u32
    {
        none                                = 0x0,
        base_map_alpha_modulates_reflect    = 0x1,
        base_map_color_modulates_background = 0x2,
        atmospheric_fod                     = 0x4,
        draw_before_fog                     = 0x8,
    };

    u32         unknown_;
    water_flags flags;
    u32         padding_[8];

    tagref_typed_t<tag_class_t::bitm> base;

    u32 padding_2[4];

    view_props perpendicular;
    view_props parallel;

    u32 padding_3[4];

    tagref_typed_t<tag_class_t::bitm> reflection_map;

    struct ripple_t /* TODO: Find the correct layout of this structure */
    {
        scalar contribution;
        scalar anim_angle;
        scalar anim_velocity;
        Vecf2  map_offset;
        u32    map_repeats;
        u32    map_index;
    };

    u32 padding_4[4];

    struct
    {
        scalar anim_angle;
        scalar anim_velocity;
        scalar scale;

        tagref_typed_t<tag_class_t::bitm> maps;

        u32    mipmap_levels;
        scalar fade_factor;
        scalar mipmap_bias;

        u32 padding_[16];

        reflexive_t<ripple_t, V> ripples;
    } ripple;
};

struct alignas(4) shader_env : shader_base /* aka senv */
{
    enum class env_shader_type : u32
    {
        unknown,

        normal = 1,
        blended,
        blended_base_specular,
    };
    enum class diffuse_flags : u32
    {
        none                = 0x0,
        rescale_detail_maps = 0x1,
        rescale_bump_map    = 0x2,
    };

    env_shader_type shader_type;

    u32 padding_;

    struct
    {
        scalar                            spacing;
        tagref_typed_t<tag_class_t::lens> lens_flare;

        u32 unknown_2[17];
    } lens_flare;

    struct
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

    scalar unknown_1[2];
    u32    padding_2[4];

    struct
    {
        animation_function u_anim;
        scalar             u_period;
        scalar             u_scale;

        animation_function v_anim;
        scalar             v_period;
        scalar             v_scale;

        u32 padding_[2];
    } scrolling;

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
        scalar             period;
        scalar             phase;

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

    u32 padding_3[4];

    enum class specular_flags : u32
    {
        none              = 0x0,
        overbright        = 0x1,
        extra_shiny       = 0x2,
        lightmap_specular = 0x4,
    };

    struct
    {
        u32            unknown_;
        specular_flags flags;
        u32            padding_[4];
        scalar         brightness;
        Vecf3          perpendicular_color;
        Vecf3          parallel_color;

    } specular;

    u32 padding_4[26];

    enum class reflection_flags : u32
    {
        none           = 0x0,
        dynamic_mirror = 0x1,
    };

    struct
    {
        reflection_flags    flags;
        reflection_map_type type;
        scalar              lightmap_brightness;
        scalar              perpendicular_brightness;
        scalar              parallel_brightness;

        tagref_typed_t<tag_class_t::bitm> reflection;
    } reflection;
};

struct alignas(4) shader_model : shader_base /* aka soso */
{
    enum class model_flags : u32
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
    scalar      translucency;

    struct
    {
        color_src src;
    } change_color;

    u32 padding_1[14];

    struct
    {
        self_illum_flags   flags;
        color_src          color_source;
        animation_function anim_func;
        scalar             anim_period;
        Vecf3              anim_color_lower_bound;
        Vecf3              anim_color_upper_bound;
    } self_illum;

    u32 padding_2[3];

    struct
    {
        Vecf2 scale;

        tagref_typed_t<tag_class_t::bitm> base;
        u32                               padding_1[2];
        tagref_typed_t<tag_class_t::bitm> multipurpose;
        u32                               padding_2[2];

        struct
        {
            u16                               function;
            u16                               mask;
            scalar                            scale;
            tagref_typed_t<tag_class_t::bipd> map;
            scalar                            v_scale;
        } detail;
    } maps;

    texture_scrolling_animation anim_2d;
};

struct alignas(4) shader_plasma : shader_base /* TODO */
{
    u32 padding_1[2];

    struct
    {
        animation_src source;
        scalar        exponent;
    } intensity;

    struct
    {
        animation_src source;
        scalar        amount;
        scalar        exponent;
    } offset;

    u32 padding_2[9];

    struct
    {
        scalar        perpendicular_brightness;
        Vecf3         perpendicular_tint;
        scalar        parallel_brightness;
        Vecf3         parellel_tint;
        animation_src tint_src;
    } color;

    u32 padding_3[2 + 3 * 4];

    struct noise_map
    {
        scalar     anim_period;
        Vecf3      anim_dir;
        detail_map noise;
    };

    noise_map primary_noise;

    noise_map secondary_noise;
};

C_FLAGS(shader_model::model_flags, u32)

} // namespace shader
