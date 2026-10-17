#pragma once

#include "structures.h"

namespace blam::elegy {

/*!
 * \brief A start/count pair packed into 16 bits: count in the top 6,
 * start index in the low 10.
 */
struct packed_range
{
    be<u16> raw;

    inline u16 start() const
    {
        return raw.value() & 0x3FF;
    }

    inline u16 count() const
    {
        return raw.value() >> 10;
    }
};

static_assert(sizeof(packed_range) == 2);

enum class parameter_type_t : u32
{
    bitmap     = 0,
    color      = 1,
    real       = 2,
    integer    = 3,
    boolean    = 4,
    argb_color = 5,
};

struct animated_parameter_t
{
    be<u32>        type;
    string_id      input;
    string_id      range;
    be<f32>        time_period;
    data_reference function;
};

static_assert(sizeof(animated_parameter_t) == 0x24);

/*! An authored shader parameter; the compiled values are in postprocess */
struct render_method_parameter_t
{
    string_id                       name;
    be<parameter_type_t>            type;
    tag_reference                   bitmap;
    be<f32>                         real;
    be<i32>                         integer;
    be<i16>                         bitmap_flags;
    be<i16>                         filter_mode;
    be<i16>                         comparison_function;
    be<i16>                         address_mode;
    be<i16>                         address_mode_x;
    be<i16>                         address_mode_y;
    be<i16>                         anisotropy;
    be<i16>                         extern_rtt_mode;
    tag_block<animated_parameter_t> animated;
};

static_assert(offsetof(render_method_parameter_t, real) == 0x18);
static_assert(sizeof(render_method_parameter_t) == 0x3C);

/*!
 * \brief A sampler binding. Named by the template's texture list at the
 * same index; transform names the float constant holding its UV scale (xy)
 * and offset (zw).
 */
struct postprocess_texture_t
{
    tag_reference bitmap;
    be<i16>       bitmap_index;
    u8            address_mode;
    u8            filter_mode;
    u8            extern_mode;
    i8            transform;
    packed_range  blocks;
};

static_assert(sizeof(postprocess_texture_t) == 0x18);

/*! Postprocess pass ranges into the texture/constant lists */
struct postprocess_pass_t
{
    packed_range bitmaps;
    packed_range vertex_real;
    packed_range pixel_real;
};

static_assert(sizeof(postprocess_pass_t) == 0x6);

/*! Where a constant or sampler goes: D3D register, source list index */
struct routing_t
{
    be<u16> destination;
    u8      source;
    u8      type_specific;
};

static_assert(sizeof(routing_t) == 0x4);

/*!
 * \brief The compiled shader: template plus the textures and constants to
 * feed it, in the template's parameter order.
 */
struct render_method_postprocess_t
{
    tag_reference                    shader_template;
    tag_block<postprocess_texture_t> textures;
    tag_block<be<Vecf4>>             float_constants;
    tag_block<be<i32>>               integer_constants;
    be<u32>                          boolean_constants;
    tag_block<packed_range>          entry_points;
    tag_block<postprocess_pass_t>    passes;
    tag_block<routing_t>             routing;
    tag_block<animated_parameter_t>  overlays;
    be<i32>                          blend_mode;
    be<u32>                          flags;
    u32                              unknown_0;
    std::array<be<i16>, 8>           queryable_properties;
};

static_assert(offsetof(render_method_postprocess_t, boolean_constants) == 0x34);
static_assert(offsetof(render_method_postprocess_t, blend_mode) == 0x68);
static_assert(sizeof(render_method_postprocess_t) == 0x84);

/*!
 * \brief Common head of every render method group (rmsh, rmtr, rmhg,
 * rmcs, rmfl, rmct, rmw, rmd, ...): which option the shader picked in each
 * category of its definition, the authored parameters and the compiled
 * postprocess. Groups add their own fields after it.
 */
struct render_method_t
{
    tag_reference                          definition;
    tag_block<be<i16>>                     options;
    tag_block<render_method_parameter_t>   parameters;
    tag_block<render_method_postprocess_t> postprocess;
    be<u16>                                flags;
    u8                                     sort_layer;
    u8                                     runtime_flags;
    be<i32>                                custom_fog_setting;
    be<i32>                                prediction_atom;
};

static_assert(offsetof(render_method_t, options) == 0x10);
static_assert(offsetof(render_method_t, postprocess) == 0x28);
static_assert(sizeof(render_method_t) == 0x40);

/*! rmsh, rmhg, rmcs, rmfl, rmct also name a global material */
struct shader_t
{
    render_method_t base;
    string_id       material;
};

static_assert(sizeof(shader_t) == 0x44);

struct render_method_option_t
{
    string_id     name;
    tag_reference option;
    string_id     vertex_function;
    string_id     pixel_function;
};

static_assert(sizeof(render_method_option_t) == 0x1C);

struct render_method_category_t
{
    string_id                         name;
    tag_block<render_method_option_t> options;
    string_id                         vertex_function;
    string_id                         pixel_function;
};

static_assert(sizeof(render_method_category_t) == 0x18);

/*! render_method_definition ("rmdf"): the option categories of a family */
struct render_method_definition_t
{
    static constexpr std::string_view group = "rmdf";

    tag_reference                       global_options;
    tag_block<render_method_category_t> categories;
    tag_block<std::array<u8, 0x10>>     entry_points;
    tag_block<std::array<u8, 0x10>>     vertex_types;
    tag_reference                       shared_pixel_shaders;
    tag_reference                       shared_vertex_shaders;
    be<u32>                             flags;
    be<i32>                             version;
};

static_assert(offsetof(render_method_definition_t, shared_pixel_shaders) == 0x34);
static_assert(sizeof(render_method_definition_t) == 0x5C);

struct template_pass_t
{
    packed_range bitmaps;
    packed_range vertex_real;
    packed_range vertex_integer;
    packed_range vertex_boolean;
    packed_range pixel_real;
    packed_range pixel_integer;
    packed_range pixel_boolean;
    packed_range extern_bitmaps;
    packed_range extern_vertex_real;
    packed_range extern_vertex_integer;
    packed_range extern_pixel_real;
    packed_range extern_pixel_integer;
    be<i32>      alpha_blend_mode;
};

static_assert(sizeof(template_pass_t) == 0x1C);

/*!
 * \brief render_method_template ("rmt2"): the compiled vertex/pixel shader
 * pair, plus parameter names that give the postprocess lists their meaning.
 */
struct render_method_template_t
{
    static constexpr std::string_view group = "rmt2";

    tag_reference              vertex_shader;
    tag_reference              pixel_shader;
    be<u32>                    entry_points_available;
    tag_block<packed_range>    entry_points;
    tag_block<template_pass_t> passes;
    tag_block<routing_t>       routing;
    tag_block<string_id>       float_constants;
    tag_block<string_id>       integer_constants;
    tag_block<string_id>       boolean_constants;
    tag_block<string_id>       textures;
};

static_assert(offsetof(render_method_template_t, passes) == 0x30);
static_assert(offsetof(render_method_template_t, textures) == 0x6C);

} // namespace blam::elegy
