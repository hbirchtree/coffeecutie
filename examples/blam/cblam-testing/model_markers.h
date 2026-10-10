#pragma once

#include "selected_version.h"

#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_tag_ref.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <optional>
#include <string_view>

/* Model space transform of a named marker on a mod2, in the bind pose */
inline std::optional<typing::vector_types::Matf4> bind_marker(
    blam::tag_t const& model, std::string_view name, blam::map_ptr const& magic)
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
        auto const& at = instances.value()[0];
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
        return node * glm::translate(Matf4(1), at.position) *
               glm::mat4_cast(glm::conjugate(rotation));
    }
    return std::nullopt;
}
