#pragma once

#include "blam_files.h"
#include "components.h"
#include "physics.h"
#include "selected_version.h"

#include <blam/volta/blam_shaders.h>
#include <coffee/components/entity_container.h>

/* Projectiles in flight: speed, gravity, what they hit and how they end */
void alloc_projectiles(compo::EntityContainer& e);

/* What a ray struck is made of: a surface's shader, a body part's material,
 * else a guess by what it is */
template<typename Proxy>
blam::shader::radiosity_properties::physics_material struck_material(
    Proxy& p, Physics::Raycast const& ray, BlamFiles<halo_version> const& f)
{
    using material_t = blam::shader::radiosity_properties::physics_material;
    if(ray.entity_id == 0)
    {
        if(!ray.shader || !ray.shader->valid())
            return material_t::stone;
        blam::tag_index_view<halo_version> index(f.container);
        auto shader = index.find(*ray.shader);
        if(shader == index.end())
            return material_t::stone;
        auto surface = (*shader).template data<
            blam::shader::radiosity_properties>(f.container.magic);
        return surface.has_value() ? surface.value()[0].physics
                                   : material_t::stone;
    }
    /* The struck part's own, an energy shield's included */
    if(ray.material_type < blam::scn::material_type_count)
        return static_cast<material_t>(ray.material_type);
    std::shared_ptr<CollisionGeometry const> geometry;
    if(auto const* info = p.template get<PlayerInfo>(ray.entity_id))
        geometry = info->biped.collision;
    else if(auto const* physics = p.template get<ObjectPhysics>(ray.entity_id))
        geometry = physics->collision;
    if(geometry && ray.part >= 0 && static_cast<size_t>(ray.part) < geometry->nodes.size() &&
       geometry->nodes[ray.part].material_type < blam::scn::material_type_count)
        return static_cast<material_t>(geometry->nodes[ray.part].material_type);
    if(p.template get<PlayerInfo>(ray.entity_id))
        return material_t::cyborg_armor;
    if(auto const* physics = p.template get<ObjectPhysics>(ray.entity_id);
       physics && physics->drive)
        return material_t::metal_thicc;
    return material_t::metal_thin;
}
