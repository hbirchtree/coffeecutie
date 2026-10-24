#pragma once

#include "../physics.h"
#include "types.h"

#include <blam/common/collision_bsp.h>

#if defined(FEATURE_ENABLE_BULLET3)
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#endif

#include <cmath>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace world {

/* A level's collision as one static body in the physics world, under `id` */
inline void submit_static_collision(PhysicsBus& bus, std::shared_ptr<CollisionMesh> mesh, u64 id)
{
#if defined(FEATURE_ENABLE_BULLET3)
    if(!mesh || mesh->indices.empty())
        return;
    auto body        = std::make_shared<Physics::BodyCreationPrebuilt>();
    body->entity_id  = id;
    body->mesh_iface = std::make_unique<btTriangleIndexVertexArray>();
    btIndexedMesh part;
    part.m_numTriangles        = static_cast<int>(mesh->indices.size() / 3);
    part.m_triangleIndexBase   = reinterpret_cast<unsigned char const*>(mesh->indices.data());
    part.m_triangleIndexStride = 3 * sizeof(i32);
    part.m_numVertices         = static_cast<int>(mesh->vertices.size());
    part.m_vertexBase          = reinterpret_cast<unsigned char const*>(mesh->vertices.data());
    part.m_vertexStride        = sizeof(Vecf3);
    part.m_vertexType          = PHY_FLOAT;
    body->mesh_iface->addIndexedMesh(part, PHY_INTEGER);
    body->shape              = std::make_unique<btBvhTriangleMeshShape>(body->mesh_iface.get(), true);
    body->triangle_materials = mesh->materials;
    body->keep_alive         = std::move(mesh);
    Physics::Event event{.type = Physics::Event::BodyCreationPrebuilt};
    bus.process(event, body.get());
#else
    (void)bus, (void)mesh, (void)id;
#endif
}

/*!
 * Appends a collision BSP's surfaces to `out` as fanned triangles, its
 * vertices (`point_of` each) moved by `transform`. Vertices at infinity bound
 * the BSP and a quantized BVH cannot hold them, so surfaces touching one are
 * left out, as are those `skip` names. `material_of(surface)` gives each
 * triangle's material when the mesh carries them.
 */
template<typename Tr, typename Vertices, typename PointOf, typename Skip, typename MaterialOf = std::nullptr_t>
inline void append_fanned(
    CollisionMesh&                               out,
    Matf4 const&                                 transform,
    Vertices const&                              vertices,
    PointOf&&                                    point_of,
    semantic::Span<typename Tr::surface_t const> surfaces,
    semantic::Span<typename Tr::edge_t const>    edges,
    Skip&&                                       skip,
    MaterialOf                                   material_of = nullptr)
{
    i32 const         vbase = static_cast<i32>(out.vertices.size());
    std::vector<bool> finite;
    finite.reserve(vertices.size());
    for(auto const& v : vertices)
    {
        Vecf3 const point = point_of(v);
        finite.push_back(std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z));
        out.vertices.push_back(finite.back() ? Vecf3(transform * Vecf4(point, 1.f)) : Vecf3{});
    }
    blam::collision_bsp::fan_surfaces<Tr>(surfaces, edges, finite, skip, [&](i32 a, i32 b, i32 c, u32 s) {
        out.indices.insert(out.indices.end(), {vbase + a, vbase + b, vbase + c});
        if constexpr(!std::is_same_v<MaterialOf, std::nullptr_t>)
            out.materials.push_back(material_of(s));
    });
}

} // namespace world
