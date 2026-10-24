#pragma once

#include "selected_version.h"

#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_tag_ref.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <optional>
#include <string_view>

/* A marker in the bind pose, and the node it rides on */
struct bound_marker_t
{
    typing::vector_types::Matf4 transform;
    libc_types::i32             node{-1};
};

/* Model space transform of a named marker on a mod2, in the bind pose;
 * markers with several instances (twin barrels) wrap around `instance` */
inline std::optional<bound_marker_t> bind_marker_node(
    blam::tag_t const&   model,
    std::string_view     name,
    blam::map_ptr const& magic,
    libc_types::u32      instance = 0)
{
    using typing::vector_types::Matf4;
    using typing::vector_types::Quatf;

    auto mod2 = model.data<blam::mod2::header<halo_version>>(magic);
    if(!mod2.has_value())
        return std::nullopt;
    auto markers = mod2.value()[0].markers.data(magic);
    auto bones   = mod2.value()[0].bones.data(magic);
    if(!markers.has_value() || !bones.has_value())
        return std::nullopt;
    for(auto const& marker : markers.value())
    {
        if(marker.name.str() != name)
            continue;
        auto instances = marker.instances.data(magic);
        if(!instances.has_value() || instances.value().empty())
            return std::nullopt;
        auto const& at = instances.value()[instance % instances.value().size()];
        /* Bind pose of its node, the way caching.cpp builds inv_bind */
        Matf4 node(1);
        for(libc_types::i32 i = at.node_idx;
            i >= 0 && static_cast<size_t>(i) < bones.value().size();)
        {
            auto const& b = bones.value()[i];
            node = glm::translate(Matf4(1), b.translation) *
                   glm::mat4_cast(glm::conjugate(b.rotation)) * node;
            i = b.parent == blam::mod2::bone::invalid_bone ? -1 : b.parent;
        }
        Quatf const rotation(
            at.rotation.w, at.rotation.x, at.rotation.y, at.rotation.z);
        return bound_marker_t{
            node * glm::translate(Matf4(1), at.position) *
                glm::mat4_cast(glm::conjugate(rotation)),
            at.node_idx,
        };
    }
    return std::nullopt;
}

/* How many instances a named marker has, 0 if none */
inline libc_types::u32 marker_instances(
    blam::tag_t const& model, std::string_view name, blam::map_ptr const& magic)
{
    auto mod2 = model.data<blam::mod2::header<halo_version>>(magic);
    if(!mod2.has_value())
        return 0;
    auto markers = mod2.value()[0].markers.data(magic);
    if(!markers.has_value())
        return 0;
    for(auto const& marker : markers.value())
        if(marker.name.str() == name)
        {
            auto instances = marker.instances.data(magic);
            return instances.has_value() ? static_cast<libc_types::u32>(instances.value().size()) : 0;
        }
    return 0;
}

inline std::optional<typing::vector_types::Matf4> bind_marker(
    blam::tag_t const&   model,
    std::string_view     name,
    blam::map_ptr const& magic,
    libc_types::u32      instance = 0)
{
    if(auto marker = bind_marker_node(model, name, magic, instance))
        return marker->transform;
    return std::nullopt;
}
