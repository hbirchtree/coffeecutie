#pragma once

#include "structures.h"

#include <array>

namespace blam::dimeter::anim {

/*!
 * jmad, the model animation graph. Layout from Assembly's Halo2Xbox
 * jmad.xml; the keyframes live outside the tag, in raw resource groups the
 * animations point into (16-byte aligned).
 *
 * An animation's data is, back to back with the sizes in data_sizes_t:
 * the static codec stream (default_data, one frame of the nodes that hold
 * still), the animated codec stream (compressed), static node flags,
 * animated node flags, then movement. Each flag group is three bit arrays
 * (rotation, translation, scale) of equal size, the nth set bit naming the
 * model node of the nth track. Same codecs and layout as Halo 3; verified
 * over every animation of the Xbox maps.
 */

struct alignas(4) skeleton_node_t
{
    string_id                    name;
    block_index<skeleton_node_t> next_sibling;
    block_index<skeleton_node_t> first_child;
    block_index<skeleton_node_t> parent;
    u8                           model_flags; /* TODO: flags */
    u8                           joint_flags; /* TODO: flags */
    Vecf3                        base_vector;
    f32                          vector_range;
    f32                          z_position;
};

static_assert(sizeof(skeleton_node_t) == 0x20);

enum class animation_type_t : u8
{
    base,
    overlay,
    replacement,
};

/*! Root motion stored with the frames: f32 per frame of each, so 8, 12
 * or 16 bytes a frame */
enum class frame_info_t : u8
{
    none,
    dx_dy,
    dx_dy_dyaw,
    dx_dy_dz_dyaw,
};

struct alignas(2) frame_event_t
{
    i16 type;
    i16 frame;
};

struct alignas(4) sound_event_t
{
    i16       sound;
    i16       frame;
    string_id marker;
};

struct alignas(2) effect_event_t
{
    i16 effect;
    i16 frame;
};

struct alignas(4) object_space_parent_t
{
    i16                 node;
    u16                 component_flags; /* TODO: flags */
    std::array<i16, 4>  rotation;
    Vecf3               translation;
    f32                 scale;
};

static_assert(sizeof(object_space_parent_t) == 0x1C);

/*! First byte of a codec stream */
enum class codec_t : u8
{
    none,
    uncompressed_static,      /*!< static stream: one frame */
    uncompressed_animated,    /*!< fullframe, f32 x4 rotations */
    quantized_rotation_only,  /*!< fullframe, i16 x4 rotations */
    byte_keyframe,            /*!< keyframes, u8 times */
    word_keyframe,            /*!< keyframes, u16 times */
    reverse_byte_keyframe,    /*!< same layout as byte_keyframe */
    reverse_word_keyframe,    /*!< same layout as word_keyframe */
    blend_screen,             /*!< fullframe, f32 x4 rotations */
};

/*!
 * Common head of a codec stream. Rotations follow the header, i16 x4
 * (/32767) unless noted; translations f32 x3 and scales f32 sit at the
 * offsets given, all relative to the stream start.
 */
struct alignas(4) codec_header_t
{
    codec_t type;
    u8      rotation_count;
    u8      translation_count;
    u8      scale_count;
    f32     error;
    f32     compression_rate;
    u32     translation_offset;
    u32     scale_offset;
};

static_assert(sizeof(codec_header_t) == 20);

/*! Static and fullframe codecs: per node, every frame */
struct alignas(4) fullframe_header_t
{
    codec_header_t base;
    u32            rotation_stride; /*!< Bytes per node; 0 in static streams */
    u32            translation_stride;
    u32            scale_stride;
};

static_assert(sizeof(fullframe_header_t) == 32);

/*!
 * Keyframe codecs: after the header one u32 per track (rotations, then
 * translations, then scales) of (first key << 12) | key count, indexing
 * that component's time table (u8 or u16 frames, from 0, ascending) and
 * value table.
 */
struct alignas(4) keyframe_header_t
{
    codec_header_t     base;
    std::array<u32, 3> time_offset;  /*!< rotation, translation, scale */
    std::array<u32, 3> value_offset; /*!< rotation, translation, scale */
    u32                unknown_0;
};

static_assert(sizeof(keyframe_header_t) == 48);

/*! Byte sizes of the sections of an animation's data; static streams with
 * translations or scales are padded to 16 bytes */
struct alignas(4) data_sizes_t
{
    u8  static_node_flags;
    u8  animated_node_flags;
    i16 movement;
    i16 pill_offset;
    i16 default_data; /*!< The static codec stream */
    i32 uncompressed;
    i32 compressed; /*!< The animated codec stream */
};

static_assert(sizeof(data_sizes_t) == 0x10);

struct alignas(4) animation_t
{
    string_id                         name;
    i32                               node_list_checksum;
    i32                               production_checksum;
    i32                               import_checksum;
    animation_type_t                  type;
    frame_info_t                      frame_info;
    i8                                blend_screen;
    u8                                node_count;
    i16                               frame_count_raw;
    u8                                internal_flags;   /* TODO: flags */
    u8                                production_flags; /* TODO: flags */
    u16                               playback_flags;   /* TODO: flags */
    u8                                desired_compression;
    u8                                current_compression;
    f32                               weight;
    i32                               unknown_0;
    i16                               resource_group_raw;
    i16                               unknown_1;
    i32                               resource_group_offset_raw;
    i16                               unknown_2;
    i16                               loop_frame;
    block_index<animation_t>          parent_animation;
    block_index<animation_t>          next_animation;
    std::array<u8, 8>                 unknown_3;
    data_sizes_t                      data_sizes;
    tag_block<frame_event_t>          frame_events;
    tag_block<sound_event_t>          sound_events;
    tag_block<effect_event_t>         effect_events;
    tag_block<object_space_parent_t>  object_space_parents;

    inline i16 frame_count() const
    {
        return from_le(frame_count_raw);
    }

    inline i16 resource_group() const
    {
        return from_le(resource_group_raw);
    }

    /*! Where this animation's data starts inside its resource group */
    inline i32 resource_group_offset() const
    {
        return from_le(resource_group_offset_raw);
    }
};

static_assert(offsetof(animation_t, resource_group_raw) == 0x24);
static_assert(offsetof(animation_t, frame_events) == 0x4C);
static_assert(sizeof(animation_t) == 0x6C);

struct alignas(4) node_data_t
{
    string_id name;
    Vecf4     rotation;
    Vecf3     translation;
    f32       scale;
    Vecf3     min_bounds;
    Vecf3     max_bounds;
};

static_assert(sizeof(node_data_t) == 0x3C);

/*! A raw blob holding the data of several animations */
struct alignas(4) resource_group_t
{
    datum_index owner;
    i32         size_raw;
    raw_ref     data;
    i32         unknown_0;
    i32         unknown_1;

    inline i32 size() const
    {
        return from_le(size_raw);
    }
};

static_assert(sizeof(resource_group_t) == 0x14);

struct alignas(4) header_t
{
    tag_reference<tag_class_t::jmad> parent;
    u8                               inheritance_flags; /* TODO: flags */
    u8                               private_flags;     /* TODO: flags */
    i16                              codec_pack;
    tag_block<skeleton_node_t>       skeleton;
    tag_block<u8>                    sounds;        /*!< elem 0xC */
    tag_block<u8>                    effects;       /*!< elem 0xC */
    tag_block<u8>                    blend_screens; /*!< elem 0x1C */
    tag_block<animation_t>           animations;
    tag_block<u8>                    modes;             /*!< elem 0x14 */
    tag_block<u8>                    vehicle_suspension; /*!< elem 0x28 */
    tag_block<u8>                    object_overlays;   /*!< elem 0x14 */
    tag_block<u8>                    inheritance;       /*!< elem 0x20 */
    tag_block<u8>                    weapons;           /*!< elem 0x8 */
    std::array<u32, 8>               left_arm_nodes;
    std::array<u32, 8>               right_arm_nodes;
    data_reference<>                 last_import_results;
    tag_block<node_data_t>           node_data;
    tag_block<resource_group_t>      resource_groups;
    tag_block<u8>                    unknown_0; /*!< elem 0x18 */
};

static_assert(offsetof(header_t, animations) == 0x2C);
static_assert(offsetof(header_t, left_arm_nodes) == 0x5C);
static_assert(offsetof(header_t, node_data) == 0xA4);
static_assert(sizeof(header_t) == 0xBC);

} // namespace blam::dimeter::anim
