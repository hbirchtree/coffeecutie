#pragma once

#include "map.h"
#include "physics.h"
#include "shader.h"
#include "structure.h"

#include <algorithm>
#include <optional>
#include <ranges>
#include <vector>

namespace blam::elegy {

/*! A value with the name its template or table gives it */
template<typename T>
struct named_t
{
    std::string_view name;
    T const&         value;
};

/*!
 * \brief An object (obje: scen, bloc, weap, ...) through its model (hlmt): the
 * render model that draws it and the physics model it moves by. Members are
 * empty from where the chain stops.
 */
struct object_view
{
    tagged<object_t>              object;
    tagged<model_t>               model;
    tagged<render_model_t>        render_model;
    tagged<phys::physics_model_t> physics_model;

    static inline object_view of(map_container const& map, tag_reference<> const& object)
    {
        object_view out{.object = map.get<object_t>(object)};
        if(!out.object)
            return out;
        out.model = map.get<model_t>(out.object->model);
        if(!out.model)
            return out;
        out.render_model  = map.get<render_model_t>(out.model->render_model);
        out.physics_model = map.get<phys::physics_model_t>(out.model->physics_model);
        return out;
    }

    /*! Forge and spawn helpers, never drawn in a game */
    inline bool editor_only(map_container const& map) const
    {
        return object && object->editor_only(map.magic());
    }
};

/*! The meshes a render model draws with by default: the first permutation
 * of each region */
inline std::vector<mesh_t const*> default_meshes(map_container const& map, render_model_t const& model)
{
    std::vector<mesh_t const*> out;
    auto                       meshes = map.span(model.geometry.meshes);
    for(auto const& region : map.span(model.regions))
    {
        auto perms = map.span(region.permutations);
        if(perms.empty() || !perms[0].mesh.valid())
            continue;
        i16 const first = perms[0].mesh.value();
        i16 const count = std::max<i16>(perms[0].mesh_count.value(), 1);
        for(i16 m = first; m < first + count && static_cast<size_t>(m) < meshes.size(); m++)
            out.push_back(&meshes[static_cast<size_t>(m)]);
    }
    return out;
}

/*! A node's place relative to its parent */
struct node_pose_t
{
    Quatf rotation{1.f, 0.f, 0.f, 0.f};
    Vecf3 translation{};
    f32   scale{1.f};
};

/*! A render model's nodes in their default pose */
std::vector<node_pose_t> default_pose(map_container const& map, render_model_t const& model);

/*! A placement's stored rotations over a pose; its translations are left
 * as they are */
void apply_orientations(
    map_container const& map, node_orientations_t const& stored, std::vector<node_pose_t>& pose);

/*! Per node: its frame in object space under `pose` */
std::vector<Matf4> node_matrices(
    map_container const& map, render_model_t const& model, std::vector<node_pose_t> const& pose);

/*! Per node: object space from the default pose to `pose` (world times the
 * inverse bind matrix), what skinned vertices are moved by */
std::vector<Matf4> skinning_matrices(
    map_container const& map, render_model_t const& model, std::vector<node_pose_t> const& pose);

/*! A marker's frame in object space, from its node's frame (node_matrices);
 * the first marker of the group by that name */
std::optional<Matf4> marker_frame(
    map_container const& map, render_model_t const& model, string_id name, Span<Matf4 const> nodes);

/*!
 * \brief A function's steady value, for what holds still (a light's colour
 * and intensity): the middle of a scalar function's range, the first colour
 * of a colour function. The header's first byte is the type, the third the
 * colour count; scalars are a float pair from 4, colours ARGB bytes from 4.
 */
f32   function_scalar(Span<u8 const> function);
Vecf3 function_color(Span<u8 const> function);

/*! A scenario's structure BSP by index */
inline tagged<structure_bsp_t> structure_bsp(
    map_container const& map, scenario_t const& scenario, i16 index)
{
    auto bsps = map.span(scenario.bsps);
    if(index < 0 || static_cast<size_t>(index) >= bsps.size())
        return {};
    return map.get<structure_bsp_t>(bsps[static_cast<size_t>(index)].bsp);
}

/*! A structure BSP's entry in the scenario lightmap: its render geometry
 * and baked light */
inline lightmap_bsp_t const* lightmap_of(map_container const& map, i16 bsp)
{
    for(auto const& tag : map.tags())
    {
        if(!tag.valid() || !map.is(tag, scenario_lightmap_t::group))
            continue;
        if(auto const* lightmap = map.tag_data<scenario_lightmap_t>(tag))
            for(auto const& entry : map.span(lightmap->bsps))
                if(entry.bsp.value() == bsp)
                    return &entry;
    }
    return nullptr;
}

/*! The cluster holding `point`: through the collision BSP to its leaf, and
 * the leaf's cluster; null outside the level or when the pages are absent */
inline cluster_t const* cluster_at(
    map_container const&   map,
    structure_bsp_t const& bsp,
    resource_view const&   resources,
    Vecf3 const&           point)
{
    auto const* root = resources.root_as<structure_bsp_resources_t>();
    auto        collision = root ? resources.block(root->collision_bsps)
                                 : Span<coll::collision_bsp_t const>();
    if(collision.empty())
        return nullptr;
    auto const leaf     = coll::bsp_view_t::from(resources, collision[0]).find_leaf(point);
    auto const leaves   = map.span(bsp.leaves);
    auto const clusters = map.span(bsp.clusters);
    i32 const  cluster  = leaf && *leaf < leaves.size() ? leaves[*leaf] : -1;
    return cluster >= 0 && static_cast<size_t>(cluster) < clusters.size()
               ? &clusters[static_cast<size_t>(cluster)]
               : nullptr;
}

/*! A placement with its palette entry, null when the index is out of range */
template<typename P>
struct placed_t
{
    P const&               placement;
    palette_entry_t const* entry;

    inline object_placement_t const& object() const
    {
        return placement.object;
    }
};

/*! A placement block with each placement's palette entry */
template<typename P>
inline auto placements(
    map_container const& map, tag_block<P> const& block, tag_block<palette_entry_t> const& palette)
{
    auto entries = map.span(palette);
    return map.span(block) | std::views::transform([entries](P const& placement) {
               return placed_t<P>{placement, placement.object.palette.resolve(entries)};
           });
}

/*! The placements the level creates in the structure BSPs of `bsps` (one
 * bit each, a zone set's) when it loads */
template<typename P>
inline auto placements_at_load(
    map_container const&              map,
    tag_block<P> const&               block,
    tag_block<palette_entry_t> const& palette,
    u32                               bsps)
{
    return placements(map, block, palette) | std::views::filter([bsps](placed_t<P> const& placed) {
               return placed.object().placed_at_load() && placed.object().in_bsps(bsps);
           });
}

/*!
 * \brief A render method's postprocess entry as its template lays it out:
 * textures and float constants by name, and the option picked in each of
 * the definition's categories.
 */
struct render_method_view
{
  private:
    template<typename T>
    inline auto named(Span<T const> values, std::string_view (render_method_view::*name)(size_t) const) const
    {
        return std::views::iota(size_t{0}, values.size()) |
               std::views::filter([this, name](size_t i) { return !(this->*name)(i).empty(); }) |
               std::views::transform([this, values, name](size_t i) {
                   return named_t<T>{(this->*name)(i), values[i]};
               });
    }

  public:
    map_container const*               map{nullptr};
    tagged<render_method_t>            method;
    tagged<render_method_definition_t> definition;
    render_method_postprocess_t const* postprocess{nullptr};
    tagged<render_method_template_t>   shader_template;
    Span<string_id const>              texture_names;
    Span<string_id const>              constant_names;
    Span<postprocess_texture_t const>  textures;
    Span<be<Vecf4> const>              constants;

    static render_method_view of(
        map_container const& map, tag_t const& method, size_t postprocess = 0);
    static render_method_view of(
        map_container const& map, tag_reference<> const& method, size_t postprocess = 0);

    inline explicit operator bool() const
    {
        return static_cast<bool>(method);
    }

    /*! The template's name for a slot, empty past its list */
    std::string_view texture_name(size_t index) const;
    std::string_view constant_name(size_t index) const;

    /*! Slots the template names, in order */
    inline auto named_textures() const
    {
        return named(textures, &render_method_view::texture_name);
    }

    inline auto named_constants() const
    {
        return named(constants, &render_method_view::constant_name);
    }

    postprocess_texture_t const* texture(std::string_view name) const;
    std::optional<Vecf4>         constant(std::string_view name) const;

    /*! The scale (xy) and offset (zw) a texture's transform names */
    Vecf4 transform_of(postprocess_texture_t const& texture) const;

    /*! Each category of the definition with its picked option; option 0
     * where the method picks none, empty where the pick is out of range */
    std::vector<std::pair<std::string_view, std::string_view>> options() const;
    std::string_view option(std::string_view category) const;
};

} // namespace blam::elegy
