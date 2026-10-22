#pragma once

#include "animation.h"
#include "geometry.h"
#include "lightmap.h"
#include "map.h"
#include "objects.h"
#include "scenario.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace blam::dimeter {

/*!
 * \brief An object (obje: scen, bloc, weap, ...) through its model (hlmt):
 * what draws it, what it collides and moves with, and how it animates.
 * Members are empty from where the chain stops.
 */
struct object_view
{
    tagged<obj::object_t>       object;
    tagged<obj::hlmt_t>         model;
    tagged<geo::model_header_t> render_model;
    tagged<obj::coll::model_t>  collision_model;
    tagged<obj::phmo::model_t>  physics_model;
    tagged<anim::header_t>      animation;

    template<typename V>
    static inline object_view of(map_container<V> const& map, tag_reference<> const& object)
    {
        object_view out{.object = map.template get<obj::object_t>(object)};
        if(!out.object)
            return out;
        out.model = map.template get<obj::hlmt_t>(out.object->model);
        if(!out.model)
            return out;
        out.render_model    = map.template get<geo::model_header_t>(out.model->render_model);
        out.collision_model = map.template get<obj::coll::model_t>(out.model->collision_model);
        out.physics_model   = map.template get<obj::phmo::model_t>(out.model->physics_model);
        out.animation       = map.template get<anim::header_t>(out.model->animation);
        return out;
    }

    /*! Whether the object's tag is of class `cls` */
    inline bool is(tag_class_t cls) const
    {
        return object && object.tag->matches(cls);
    }

    /*! Machines, controls and light fixtures: they move only as animated */
    inline bool is_device() const
    {
        return is(tag_class_t::mach) || is(tag_class_t::ctrl) || is(tag_class_t::lifi);
    }
};

/*! A structure BSP as the scenario lists it, located: its tag, its body and
 * the magic of the region it lives in, and its lightmap */
struct bsp_view
{
    u32                             index{0}; /*!< In the scenario's list */
    geo::structure_bsp_ref_t const* ref{nullptr};
    tag_t const*                    tag{nullptr};
    geo::bsp_header_t const*        body{nullptr};
    bsp_magic                       magic{};
    ltmp::header_t const*           lightmap{nullptr};
};

/*! The scenario's structure BSPs that locate, in order; each keeps its
 * scenario index, so one that does not leaves a gap */
template<typename V>
inline std::vector<bsp_view> structure_bsps(map_container<V> const& map)
{
    std::vector<bsp_view> out;
    auto const*           scnr_tag = map.scenario();
    auto const* scnr = scnr_tag ? scnr_tag->template data<scn::scenario_t>(map.magic()) : nullptr;
    if(!scnr)
        return out;
    auto const refs = map.span(scnr->structure_bsps);
    for(size_t i = 0; i < refs.size(); i++)
    {
        auto const located = locate_bsp(map, refs[i]);
        if(!located.body)
            continue;
        out.push_back({
            .index    = static_cast<u32>(i),
            .ref      = &refs[i],
            .tag      = map.tag_at(refs[i].bsp.tag_id),
            .body     = located.body,
            .magic    = located.magic,
            .lightmap = located.lightmap,
        });
    }
    return out;
}

/*! A graph's first animation of that name */
template<typename V>
inline anim::animation_t const* find_animation(
    map_container<V> const& map, anim::header_t const& graph, std::string_view name)
{
    for(auto const& a : map.span(graph.animations))
        if(map.string(a.name) == name)
            return &a;
    return nullptr;
}

/*! A graph's own default pose, one per skeleton node, from its node data */
template<typename V>
inline std::vector<anim::node_pose_t> graph_pose(
    map_container<V> const& map, anim::header_t const& graph, size_t nodes)
{
    std::vector<anim::node_pose_t> out(nodes);
    auto const                     data = map.span(graph.node_data);
    for(size_t i = 0; i < nodes && i < data.size(); i++)
    {
        auto const& d = data[i];
        out[i]        = {
            .rotation = glm::normalize(Quatf(
                from_le(d.rotation.w), from_le(d.rotation.x), from_le(d.rotation.y), from_le(d.rotation.z))),
            .translation = Vecf3(from_le(d.translation.x), from_le(d.translation.y), from_le(d.translation.z)),
            .scale       = from_le(d.scale),
        };
    }
    return out;
}

/*! An animation's data: its resource group from the animation's offset on,
 * nullopt when the group or offset is out of range */
template<typename V>
inline std::optional<raw_slice> animation_bytes(
    map_container<V> const& map,
    raw_pool const&         pool,
    anim::header_t const&   graph,
    anim::animation_t const& animation)
{
    auto const groups = map.span(graph.resource_groups);
    i16 const  group  = animation.resource_group();
    if(group < 0 || static_cast<size_t>(group) >= groups.size())
        return std::nullopt;
    auto const& g   = groups[static_cast<size_t>(group)];
    auto        raw = pool.resolve(map.magic(), &g, g.data, static_cast<u32>(std::max(g.size(), 0)));
    auto const  at  = static_cast<size_t>(std::max(animation.resource_group_offset(), 0));
    if(!raw || at >= raw->size())
        return std::nullopt;
    return raw_slice{raw->bytes.subspan(at), raw->keep};
}

/*! Decodes an animation over `defaults` (one per skeleton node) */
template<typename V>
inline std::optional<anim::decoded_t> decode_animation(
    map_container<V> const&       map,
    raw_pool const&               pool,
    anim::header_t const&         graph,
    anim::animation_t const&      animation,
    Span<anim::node_pose_t const> defaults)
{
    auto const bytes = animation_bytes(map, pool, graph, animation);
    if(!bytes)
        return std::nullopt;
    return anim::decode(animation, bytes->bytes, defaults);
}

/*! A render model's nodes at rest, from each node's own place */
template<typename V>
inline std::vector<anim::node_pose_t> model_pose(map_container<V> const& map, geo::model_header_t const& model)
{
    std::vector<anim::node_pose_t> out;
    for(auto const& n : map.span(model.nodes))
        out.push_back({
            .rotation = Quatf(from_le(n.rotation[3]), from_le(n.rotation[0]), from_le(n.rotation[1]), from_le(n.rotation[2])),
            .translation = Vecf3(from_le(n.position[0]), from_le(n.position[1]), from_le(n.position[2])),
        });
    return out;
}

/*! Per node: its frame in model space under `pose`; parents precede their
 * children. Model nodes carry no scale, so the pose's is left out. */
template<typename V>
inline std::vector<typing::vector_types::Matf4> node_matrices(
    map_container<V> const&               map,
    geo::model_header_t const&            model,
    std::vector<anim::node_pose_t> const& pose)
{
    using typing::vector_types::Matf4;
    auto const         nodes = map.span(model.nodes);
    std::vector<Matf4> out;
    for(size_t i = 0; i < nodes.size() && i < pose.size(); i++)
    {
        Matf4 const local  = glm::translate(Matf4(1.f), pose[i].translation) * glm::mat4_cast(pose[i].rotation);
        i16 const   parent = nodes[i].parent.value();
        out.push_back(parent >= 0 && static_cast<size_t>(parent) < out.size() ? out[parent] * local : local);
    }
    return out;
}

/*! Per node: its frame in model space at rest */
template<typename V>
inline std::vector<typing::vector_types::Matf4> rest_frames(
    map_container<V> const& map, geo::model_header_t const& model)
{
    return node_matrices(map, model, model_pose(map, model));
}

/*! Per node: the inverse of its rest frame, as the model stores it */
template<typename V>
inline std::vector<typing::vector_types::Matf4> inverse_bind(
    map_container<V> const& map, geo::model_header_t const& model)
{
    using typing::vector_types::Matf4;
    std::vector<Matf4> out;
    for(auto const& n : map.span(model.nodes))
    {
        auto const& m = n.inverse_transform;
        Matf4       inverse(1.f);
        inverse[0] = Vecf4(from_le(m[0]), from_le(m[1]), from_le(m[2]), 0.f);
        inverse[1] = Vecf4(from_le(m[3]), from_le(m[4]), from_le(m[5]), 0.f);
        inverse[2] = Vecf4(from_le(m[6]), from_le(m[7]), from_le(m[8]), 0.f);
        inverse[3] = Vecf4(from_le(m[9]), from_le(m[10]), from_le(m[11]), 1.f);
        out.push_back(inverse);
    }
    return out;
}

} // namespace blam::dimeter
