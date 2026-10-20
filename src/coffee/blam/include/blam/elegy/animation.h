#pragma once

#include "resources.h"
#include "views.h"

#include <optional>
#include <vector>

namespace blam::elegy {

/*! A node of an animation graph's skeleton, in render model node order */
struct animation_node_t
{
    string_id  name;
    be<i16>    next_sibling;
    be<i16>    first_child;
    be<i16>    parent;
    u8         model_flags; /* TODO: flags */
    u8         joint_flags; /* TODO: flags */
    be<Vecf3>  base_vector;
    be<f32>    vector_range;
    be<f32>    z_position;
};

static_assert(sizeof(animation_node_t) == 0x20);

/*! One animation; its data is a member of a resource group */
struct animation_t
{
    string_id             name;
    be<f32>               weight;
    be<i16>               loop_frame;
    be<u16>               playback_flags; /* TODO: flags */
    i8                    blend_screen;
    u8                    desired_compression;
    u8                    current_compression;
    u8                    node_count;
    be<i16>               frame_count;
    u8                    type;
    u8                    frame_info_type;
    be<u16>               production_flags; /* TODO: flags */
    be<u16>               internal_flags;   /* TODO: flags */
    be<i32>               node_list_checksum;
    be<i32>               production_checksum;
    be<i16>               importer_version;
    be<i16>               compressor_version;
    be<i16>               parent_animation;
    be<i16>               next_animation;
    be<i16>               resource_group;
    be<i16>               resource_group_member;
    std::array<u8, 0x48>  events; /*!< frame, sound, effect, dialogue, object space parents, foot tracking */
    be<Vecf3>             heading;
    be<f32>               average_translation;
    be<f32>               average_pivot_yaw;
};

static_assert(offsetof(animation_t, resource_group) == 0x28);
static_assert(sizeof(animation_t) == 0x88);

/*! A node's pose where no animation moves it */
struct animation_node_default_t
{
    string_id name;
    be<Vecf4> rotation; /*!< x, y, z, w */
    be<Vecf3> translation;
    be<f32>   scale;
    be<Vecf3> minimum_bounds;
    be<Vecf3> maximum_bounds;
};

static_assert(sizeof(animation_node_default_t) == 0x3C);

/*! A resource holding the data of several animations */
struct animation_resource_group_t
{
    be<i32>     reference_count;
    datum_index resource;
    be<i32>     unknown_0;
};

static_assert(sizeof(animation_resource_group_t) == 0xC);

/*! model_animation_graph ("jmad") */
struct animation_graph_t
{
    static constexpr tag_class_t group = tag_class_t::jmad;

    tag_reference<>                         parent;
    u8                                    inheritance_flags;
    u8                                    private_flags;
    be<i16>                               codec_pack;
    tag_block<animation_node_t>           nodes;
    std::array<u8, 0x30>                  unknown_0;
    tag_block<animation_t>                animations;
    std::array<u8, 0x90>                  unknown_1;
    tag_block<animation_node_default_t>   node_defaults;
    tag_block<animation_resource_group_t> resource_groups;

    /*! The first animation called `name` */
    animation_t const* find(map_container const& map, std::string_view name) const;

    /*! Every node where no animation moves it */
    std::vector<node_pose_t> default_pose(map_container const& map) const;

    /*! Per node: its frame in object space under `pose` */
    std::vector<Matf4> node_matrices(map_container const& map, std::vector<node_pose_t> const& pose) const;
};

static_assert(offsetof(animation_graph_t, nodes) == 0x14);
static_assert(offsetof(animation_graph_t, animations) == 0x50);
static_assert(offsetof(animation_graph_t, node_defaults) == 0xEC);
static_assert(offsetof(animation_graph_t, resource_groups) == 0xF8);

/*! Byte sizes of the sections of an animation's data: the static codec
 * stream, the animated one, then the static and animated node flags */
struct animation_data_sizes_t
{
    u8      static_node_flags;
    u8      animated_node_flags;
    be<i16> movement;
    be<i16> pill_offset;
    be<i16> default_data; /*!< The static codec stream */
    be<i32> uncompressed;
    be<i32> compressed; /*!< The animated codec stream */
};

static_assert(sizeof(animation_data_sizes_t) == 0x10);

/*! One animation's data in a resource group */
struct animation_member_t
{
    be<i32>                index;
    be<i32>                checksum;
    be<i16>                frame_count;
    u8                     node_count;
    u8                     movement_type;
    animation_data_sizes_t sizes;
    data_reference         data;
};

static_assert(sizeof(animation_member_t) == 0x30);

/*! A resource group's definition */
struct animation_resource_t
{
    resource_block<animation_member_t> members;
};

/*! Every frame of an animation, frame-major */
struct decoded_animation_t
{
    u32                      frames{0};
    u32                      nodes{0};
    std::vector<node_pose_t> poses;

    inline Span<node_pose_t const> frame(u32 index) const
    {
        return Span<node_pose_t const>(poses).subspan(std::min(index, frames - 1) * nodes, nodes);
    }
};

/*! Decodes a member's codec streams over `defaults` (one per node); nodes no
 * track names keep theirs */
std::optional<decoded_animation_t> decode_animation(
    resource_view const& view, animation_member_t const& member, Span<node_pose_t const> defaults);

/*! Decodes an animation of `graph`, loading its resource group; nodes it
 * does not move keep `defaults`, or the graph's own when none are given */
std::optional<decoded_animation_t> decode_animation(
    map_container const&     map,
    resource_loader&         loader,
    animation_graph_t const& graph,
    animation_t const&       animation,
    Span<node_pose_t const>  defaults = {});

/*! The graph's nodes in the default pose of the render models with the same
 * node names, the first model naming a node giving its pose */
std::vector<node_pose_t> default_pose(
    map_container const& map, animation_graph_t const& graph, Span<render_model_t const* const> models);

/*! Per node of `model`: its skinning matrix where `names` (another
 * skeleton's nodes) put it at `frames`; nodes not named stay in the
 * default pose */
std::vector<Matf4> skinning_matrices(
    map_container const&     map,
    render_model_t const&    model,
    Span<string_id const>    names,
    Span<Matf4 const>        frames);

} // namespace blam::elegy
