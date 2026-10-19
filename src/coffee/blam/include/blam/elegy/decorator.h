#pragma once

#include "geometry.h"
#include "structure.h"

#include <vector>

namespace blam::elegy {

enum class decorator_render_flags_t : u8
{
    none               = 0x0,
    two_sided          = 0x1,
    no_light_sampling  = 0x2, /*!< Don't sample light through geometry */
};
C_FLAGS(decorator_render_flags_t, u8);

enum class decorator_shader_t : u8
{
    wind_dynamic_lights   = 0,
    still_dynamic_lights  = 1,
    still_no_lights       = 2,
    still_sun_light_only  = 3,
    wavy_dynamic_lights   = 4,
    shaded_dynamic_lights = 5,
};

/*! One kind of decorator in a set: a mesh of the set's render model */
struct decorator_type_t
{
    be<i32>                index;
    be<i32>                mesh;
    be<u32>                flags; /* TODO: flags */
    bounds_f               scale;
    bounds_f               tilt;
    bounds_f               wind;
    std::array<be<f32>, 3> color_0;
    std::array<be<f32>, 3> color_1;
    std::array<be<f32>, 3> color_2;
    bounds_f               ground_tint;
    bounds_f               hover;
    be<f32>                minimum_distance;
};

static_assert(sizeof(decorator_type_t) == 0x5C);

/*! decorator_set ("dctr"): plants and debris scattered over a BSP, all
 * sharing one texture */
struct decorator_set_t
{
    static constexpr std::string_view group = "dctr";

    tag_reference                    render_model;
    tag_block<string_id>             instance_names;
    be<i32>                          instance_name_count;
    tag_reference                    texture;
    decorator_render_flags_t         render_flags;
    decorator_shader_t               shader;
    u8                               light_sampling;
    u8                               unknown_0;
    std::array<be<f32>, 4>           translucency; /*!< a, b, c, overall */
    be<Vecf2>                        wavelength;
    be<f32>                          wave_speed;
    be<f32>                          wave_frequency;
    be<f32>                          shaded_dark;
    be<f32>                          shaded_bright;
    std::array<u32, 2>               unused_0;
    be<f32>                          start_fade;
    be<f32>                          end_fade;
    be<f32>                          early_cull;
    be<f32>                          cull_block_size;
    tag_block<decorator_type_t>      types;
};

static_assert(offsetof(decorator_set_t, texture) == 0x20);
static_assert(offsetof(decorator_set_t, start_fade) == 0x64);
static_assert(offsetof(decorator_set_t, types) == 0x74);

/*! One placed decorator, decoded */
struct decorator_instance_t
{
    Vecf3 position;
    Quatf rotation; /*!< Not normalized: its squared length is the scale */
    Vecf3 light;    /*!< Baked ambient light */
    u8    type{0};  /*!< Into decorator_set_t::types */
    f32   motion{0.f}; /*!< How much wind moves it; also its sun intensity */
};

/*!
 * \brief A cluster's decorator group, decoded from the decorator geometry's
 * instance buffer: 16 bytes each, big-endian u16 x/y/z steps over the
 * group's bounds and a u16 of type (high byte) and motion; a signed byte
 * quaternion, x in the last byte; an RGBE light, exponent first.
 */
std::vector<decorator_instance_t> decorator_instances(resource_view const& view, decorator_group_t const& group);

} // namespace blam::elegy
