#include <blam/elegy/views.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace blam::elegy {

Quatf object_placement_t::orientation() const
{
    Vecf3 const angles = rotation;
    return Quatf(Vecf3(0, 0, 1) * angles.x) * Quatf(Vecf3(0, 1, 0) * angles.y) *
           Quatf(Vecf3(1, 0, 0) * angles.z);
}

Matf4 object_placement_t::transform() const
{
    return glm::translate(Matf4(1.f), position.value()) * glm::mat4_cast(orientation()) *
           glm::scale(Matf4(1.f), Vecf3(scale_or_one()));
}

Matf4 instanced_geometry_t::transform() const
{
    f32 const s = scale;
    Matf4     out(1.f);
    out[0] = Vecf4(forward.value() * s, 0.f);
    out[1] = Vecf4(left.value() * s, 0.f);
    out[2] = Vecf4(up.value() * s, 0.f);
    out[3] = Vecf4(position.value(), 1.f);
    return out;
}

std::vector<node_pose_t> default_pose(map_container const& map, render_model_t const& model)
{
    std::vector<node_pose_t> out;
    for(auto const& node : map.span(model.nodes))
    {
        Vecf4 const r = node.default_rotation.value();
        out.push_back({.rotation = Quatf(r.w, r.x, r.y, r.z), .translation = node.default_translation});
    }
    return out;
}

void apply_orientations(
    map_container const& map, node_orientations_t const& stored, std::vector<node_pose_t>& pose)
{
    auto const flags  = map.span(stored.node_flags);
    auto const shorts = map.span(stored.orientations);
    i16 const  count  = stored.node_count;
    auto       bit    = [&](i32 index) {
        return index >= 0 && static_cast<size_t>(index / 8) < flags.size() &&
               (flags[static_cast<size_t>(index / 8)] & (1u << (index % 8)));
    };
    size_t next = 0;
    for(i16 node = 0; node < count; node++)
    {
        if(bit(node))
        {
            if(next + 4 > shorts.size())
                return;
            auto q = [&](size_t k) { return static_cast<f32>(shorts[next + k].value()) / 32767.f; };
            if(static_cast<size_t>(node) < pose.size())
                pose[static_cast<size_t>(node)].rotation = glm::normalize(Quatf(q(3), q(0), q(1), q(2)));
            next += 4;
        }
        if(bit(count + node))
            next += 3;
    }
}

std::vector<Matf4> node_matrices(
    map_container const& map, render_model_t const& model, std::vector<node_pose_t> const& pose)
{
    auto const         nodes = map.span(model.nodes);
    std::vector<Matf4> world(nodes.size(), Matf4(1.f));
    for(size_t i = 0; i < nodes.size() && i < pose.size(); i++)
    {
        Matf4 const local  = glm::translate(Matf4(1.f), pose[i].translation) * glm::mat4_cast(pose[i].rotation);
        i16 const   parent = nodes[i].parent.value();
        world[i] = parent >= 0 && static_cast<size_t>(parent) < i ? world[static_cast<size_t>(parent)] * local
                                                                 : local;
    }
    return world;
}

std::vector<Matf4> skinning_matrices(
    map_container const& map, render_model_t const& model, std::vector<node_pose_t> const& pose)
{
    auto const nodes = map.span(model.nodes);
    auto       out   = node_matrices(map, model, pose);
    for(size_t i = 0; i < nodes.size() && i < out.size(); i++)
    {
        auto const& node = nodes[i];
        Matf4       inverse(1.f);
        inverse[0] = Vecf4(node.inverse_forward.value(), 0.f);
        inverse[1] = Vecf4(node.inverse_left.value(), 0.f);
        inverse[2] = Vecf4(node.inverse_up.value(), 0.f);
        inverse[3] = Vecf4(node.inverse_position.value(), 1.f);
        out[i]     = out[i] * inverse;
    }
    return out;
}

std::optional<Matf4> marker_frame(
    map_container const& map, render_model_t const& model, string_id name, Span<Matf4 const> nodes)
{
    for(auto const& group : map.span(model.marker_groups))
    {
        if(group.name.raw.value() != name.raw.value())
            continue;
        for(auto const& marker : map.span(group.markers))
        {
            Vecf4 const r    = marker.rotation.value();
            Matf4 const node = marker.node >= 0 && static_cast<size_t>(marker.node) < nodes.size()
                                   ? nodes[static_cast<size_t>(marker.node)]
                                   : Matf4(1.f);
            return node * glm::translate(Matf4(1.f), marker.translation.value()) *
                   glm::mat4_cast(Quatf(r.w, r.x, r.y, r.z));
        }
    }
    return std::nullopt;
}

f32 function_scalar(Span<byte_t const> function)
{
    if(function.size() < 12)
        return 0.f;
    auto value = [&](size_t at) {
        be<f32> v;
        std::memcpy(v.raw.data(), function.data() + at, sizeof(v.raw));
        return v.value();
    };
    return (value(4) + value(8)) * .5f;
}

Vecf3 function_color(Span<byte_t const> function)
{
    if(function.size() < 8 || function[2] == 0)
        return Vecf3(1.f);
    return Vecf3(function[5], function[6], function[7]) / 255.f;
}

render_method_view render_method_view::of(
    map_container const& map, tag_t const& method, size_t postprocess)
{
    return of(map, tag_reference{.id = map.id_of(method)}, postprocess);
}

render_method_view render_method_view::of(
    map_container const& map, tag_reference const& method, size_t postprocess)
{
    render_method_view out{.map = &map, .method = map.get<render_method_t>(method)};
    if(!out.method)
        return out;
    out.definition = map.get<render_method_definition_t>(out.method->definition);
    auto posts     = map.span(out.method->postprocess);
    if(postprocess >= posts.size())
        return out;
    out.postprocess     = &posts[postprocess];
    out.shader_template = map.get<render_method_template_t>(out.postprocess->shader_template);
    if(out.shader_template)
    {
        out.texture_names  = map.span(out.shader_template->textures);
        out.constant_names = map.span(out.shader_template->float_constants);
    }
    out.textures  = map.span(out.postprocess->textures);
    out.constants = map.span(out.postprocess->float_constants);
    return out;
}

std::string_view render_method_view::texture_name(size_t index) const
{
    return index < texture_names.size() ? map->string(texture_names[index]) : std::string_view();
}

std::string_view render_method_view::constant_name(size_t index) const
{
    return index < constant_names.size() ? map->string(constant_names[index]) : std::string_view();
}

postprocess_texture_t const* render_method_view::texture(std::string_view name) const
{
    for(auto const& [slot, texture] : named_textures())
        if(slot == name)
            return &texture;
    return nullptr;
}

std::optional<Vecf4> render_method_view::constant(std::string_view name) const
{
    for(auto const& [slot, value] : named_constants())
        if(slot == name)
            return value.value();
    return std::nullopt;
}

Vecf4 render_method_view::transform_of(postprocess_texture_t const& texture) const
{
    i16 const transform = texture.transform;
    if(transform >= 0 && static_cast<size_t>(transform) < constants.size())
        return constants[static_cast<size_t>(transform)].value();
    return Vecf4(1.f, 1.f, 0.f, 0.f);
}

std::vector<std::pair<std::string_view, std::string_view>> render_method_view::options() const
{
    std::vector<std::pair<std::string_view, std::string_view>> out;
    if(!method || !definition)
        return out;
    auto picked     = map->span(method->options);
    auto categories = map->span(definition->categories);
    for(size_t i = 0; i < categories.size(); i++)
    {
        auto      choices = map->span(categories[i].options);
        i16 const pick    = i < picked.size() ? picked[i].value() : i16(0);
        out.emplace_back(
            map->string(categories[i].name),
            pick >= 0 && static_cast<size_t>(pick) < choices.size()
                ? map->string(choices[static_cast<size_t>(pick)].name)
                : std::string_view());
    }
    return out;
}

std::string_view render_method_view::option(std::string_view category) const
{
    for(auto const& [name, choice] : options())
        if(name == category)
            return choice;
    return {};
}

} // namespace blam::elegy
