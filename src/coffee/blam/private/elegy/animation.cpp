#include <blam/elegy/animation.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace blam::elegy {

animation_t const* animation_graph_t::find(map_container const& map, std::string_view name) const
{
    for(auto const& animation : map.span(animations))
        if(map.string(animation.name) == name)
            return &animation;
    return nullptr;
}

std::vector<node_pose_t> animation_graph_t::default_pose(map_container const& map) const
{
    auto const               defaults = map.span(node_defaults);
    std::vector<node_pose_t> out(map.span(nodes).size());
    for(size_t i = 0; i < out.size() && i < defaults.size(); i++)
    {
        Vecf4 const r = defaults[i].rotation.value();
        out[i]        = {
                   .rotation    = glm::normalize(Quatf(r.w, r.x, r.y, r.z)),
                   .translation = defaults[i].translation,
                   .scale       = defaults[i].scale,
        };
    }
    return out;
}

std::vector<Matf4> animation_graph_t::node_matrices(map_container const& map, std::vector<node_pose_t> const& pose) const
{
    auto const         list = map.span(nodes);
    std::vector<Matf4> world(list.size(), Matf4(1.f));
    for(size_t i = 0; i < list.size() && i < pose.size(); i++)
    {
        Matf4 const local = glm::translate(Matf4(1.f), pose[i].translation) * glm::mat4_cast(pose[i].rotation) *
                            glm::scale(Matf4(1.f), Vecf3(pose[i].scale));
        i16 const parent = list[i].parent.value();
        world[i] = parent >= 0 && static_cast<size_t>(parent) < i ? world[static_cast<size_t>(parent)] * local : local;
    }
    return world;
}

std::optional<decoded_animation_t> decode_animation(
    resource_view const& view, animation_member_t const& member, Span<node_pose_t const> defaults)
{
    auto const& sizes = member.sizes;
    return anim::decode_clip<std::endian::big>(
        view.bytes(member.data),
        {
            .frames         = static_cast<u32>(std::max<i16>(member.frame_count, 1)),
            .static_size    = static_cast<size_t>(std::max<i16>(sizes.default_data, 0)),
            .animated_size  = static_cast<size_t>(std::max<i32>(sizes.compressed, 0)),
            .static_flags   = sizes.static_node_flags,
            .animated_flags = sizes.animated_node_flags,
        },
        defaults);
}

std::optional<decoded_animation_t> decode_animation(
    map_container const&     map,
    resource_loader&         loader,
    animation_graph_t const& graph,
    animation_t const&       animation,
    Span<node_pose_t const>  defaults)
{
    auto const groups = map.span(graph.resource_groups);
    i16 const  group  = animation.resource_group;
    if(group < 0 || static_cast<size_t>(group) >= groups.size())
        return std::nullopt;
    auto view = loader.load(groups[static_cast<size_t>(group)].resource);
    if(!view.has_value())
        return std::nullopt;
    auto const* root = view->root_as<animation_resource_t>();
    if(!root)
        return std::nullopt;
    auto const members = view->block(root->members);
    i16 const  member  = animation.resource_group_member;
    if(member < 0 || static_cast<size_t>(member) >= members.size())
        return std::nullopt;
    auto const own = defaults.empty() ? graph.default_pose(map) : std::vector<node_pose_t>();
    return decode_animation(view.value(), members[static_cast<size_t>(member)], defaults.empty() ? Span<node_pose_t const>(own) : defaults);
}

std::vector<node_pose_t> default_pose(
    map_container const& map, animation_graph_t const& graph, Span<render_model_t const* const> models)
{
    auto       out   = graph.default_pose(map);
    auto const nodes = map.span(graph.nodes);
    std::vector<bool> posed(out.size());
    for(auto const* model : models)
    {
        auto const pose = elegy::default_pose(map, *model);
        auto const own  = map.span(model->nodes);
        for(size_t i = 0; i < nodes.size(); i++)
            for(size_t k = 0; k < own.size() && !posed[i]; k++)
                if(own[k].name.raw.value() == nodes[i].name.raw.value())
                {
                    out[i]   = pose[k];
                    posed[i] = true;
                }
    }
    return out;
}

std::vector<Matf4> skinning_matrices(
    map_container const& map, render_model_t const& model, Span<string_id const> names, Span<Matf4 const> frames)
{
    auto const nodes = map.span(model.nodes);
    auto       out   = node_matrices(map, model, default_pose(map, model));
    for(size_t i = 0; i < nodes.size(); i++)
    {
        auto const& node = nodes[i];
        for(size_t k = 0; k < names.size() && k < frames.size(); k++)
            if(names[k].raw.value() == node.name.raw.value())
            {
                out[i] = frames[k];
                break;
            }
        Matf4 inverse(1.f);
        inverse[0] = Vecf4(node.inverse_forward.value(), 0.f);
        inverse[1] = Vecf4(node.inverse_left.value(), 0.f);
        inverse[2] = Vecf4(node.inverse_up.value(), 0.f);
        inverse[3] = Vecf4(node.inverse_position.value(), 1.f);
        out[i]     = out[i] * inverse;
    }
    return out;
}

} // namespace blam::elegy
