#pragma once

#include "../physics.h"
#include "types.h"

#if defined(FEATURE_ENABLE_BULLET3)
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#endif

#include <memory>

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

} // namespace world
