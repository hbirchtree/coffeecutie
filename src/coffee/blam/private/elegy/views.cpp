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
