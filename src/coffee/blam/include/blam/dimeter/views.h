#pragma once

#include "animation.h"
#include "geometry.h"
#include "map.h"
#include "objects.h"

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

} // namespace blam::dimeter
