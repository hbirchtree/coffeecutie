#include "physics.h"

#include "caching.h"
#include "coffee/core/CProfiling"
#include "components.h"
#include "data.h"
#include "map_marker.h"
#include "peripherals/stl/math.h"
#include "selected_version.h"

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btConcaveShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <coffee/core/debug/formatting.h>
#include <magic_enum/magic_enum.hpp>

#if defined(FEATURE_ENABLE_BULLET3)

#include <btBulletDynamicsCommon.h>

#include <chrono>
#include <limits>
#include <memory>
#include <vector>

using namespace std::chrono;

/* Physics subsystem: maintains a Bullet dynamics world whose static
 * collision geometry is built from the active BSP section's collision
 * surface mesh (winged-edge → triangle soup → btBvhTriangleMeshShape).
 *
 * Two ray-query paths exist:
 *  1. Bullet rayTest against the baked BVH (this subsystem)
 *  2. BSPItem::raycast — native recursive walk of the solid-leaf BSP,
 *     zero preprocessing, usable even when physics is compiled out
 * On section load both are run over a validation ray grid and compared.
 */
template<typename V>
using PhysicsManifest = compo::SubsystemManifest<
    type_list_t<DebugDraw, PhysicsData, PlayerCamera>,
    type_list_t<BSPCache<V>, DebugMarkers, LoadingStatus>,
    empty_list_t>;

template<typename V>
struct PhysicsSystem
    : compo::RestrictedSubsystem<PhysicsSystem<V>, PhysicsManifest<V>>
{
    using type  = PhysicsSystem<V>;
    using Proxy = compo::proxy_of<PhysicsManifest<V>>;

    PhysicsSystem()
    {
        m_config     = std::make_unique<btDefaultCollisionConfiguration>();
        m_dispatcher = std::make_unique<btCollisionDispatcher>(m_config.get());
        m_broadphase = std::make_unique<btDbvtBroadphase>();
        m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        m_world  = std::make_unique<btDiscreteDynamicsWorld>(
            m_dispatcher.get(),
            m_broadphase.get(),
            m_solver.get(),
            m_config.get());
        m_world->setGravity(btVector3(0, 0, -9.81f));
        m_world_basis.setIdentity();
        m_world_basis.setRotation(btQuaternion(
            btVector3(1, 0, 0),
            stl_types::math::pi / 2)
        );
    }

    void start_restricted(Proxy& p, compo::time_point const&)
    {
        BSPCache<V>* bsp_cache;
        LoadingStatus* loading;
        p.subsystem(bsp_cache);
        p.subsystem(loading);

        /* Rebuild on section change, but also retry while no world mesh
         * exists yet — the BSP cache is populated asynchronously after the
         * subsystem starts, and single-BSP maps never switch sections. */
        bool needs_rebuild = bsp_cache->active_section != m_built_section;
        if(!needs_rebuild && !m_world_body)
            needs_rebuild = find_section_item(*bsp_cache) != nullptr;
        if(needs_rebuild)
            rebuild_world(*bsp_cache);

        // Don't simulate before we're completely done loading
        if(loading->loading)
            return;

        // Simulate when there is anything in the world: the Halo BSP body,
        // streamed RS2 region bodies, or the debug probe. Gating solely on
        // m_world_body froze RS2-only sessions (no Halo map) — no step and
        // no probe/debug marker update.
        if(!m_world_body && m_bodies.empty() && !m_probe_body)
            return;

        m_world->stepSimulation(1.f / 60.f, 4);

        if(m_probe_body)
            update_probe_marker(p);

        if(m_probe_body && m_frame < 600 && (m_frame % 60) == 0)
        {
            auto const& org =
                m_probe_body->getWorldTransform().getOrigin();
            cDebug(
                "physics: probe sphere at ({:.2f}, {:.2f}, {:.2f}){}",
                org.x(),
                org.y(),
                org.z(),
                m_probe_body->isActive() ? "" : " [settled]");
        }

        do
        {
            DebugMarkers* markers;
            p.subsystem(markers);
            if(!markers->available())
                break;
            markers->map();
            for(auto& [entity, body] : m_bodies)
            {
                DebugDraw* debug = p.template get<DebugDraw>(entity);
                if(!debug)
                    continue;
                btRigidBody* rigid_body = body.world_body.get();
                btVector3 min, max;
                rigid_body->getAabb(min, max);
                auto const box = DebugMarkers::box_vertices(
                    {min.x(), min.y(), min.z()},
                    {max.x(), max.y(), max.z()}
                );
                debug->data.arrays = {
                    .count = 16,
                    .offset = physics_debug_point_ptr + 16,
                };
                debug->color_ptr = physics_debug_color_ptr + 1;
                markers->put_strip(
                    physics_debug_point_ptr + 16,
                    physics_debug_color_ptr + 1,
                    box,
                    Vecf3{1.f, .5f, 0.f});
            }
            markers->unmap();
        } while(false);

        for(auto const& player_ : p.template select<PlayerCamera>())
        {
            auto player = p.ref(player_);
            PlayerCamera& camera = player.template get<PlayerCamera>();
            if(!camera.mode.physics)
                continue;
            auto phys_it = m_bodies.find(player.id());
            if(phys_it == m_bodies.end())
                continue;
            entity_body& phys = (*phys_it).second;
            btVector3& origin = phys.world_body->getWorldTransform().getOrigin();
            camera.camera->position = {
                origin.x(),
                origin.y(),
                origin.z() + 0.2f,
            };
        }

        m_frame++;
    }

    void end_restricted(Proxy&, compo::time_point const&)
    {
    }

    /* Wire box following the probe sphere, drawn through the existing
     * debug line pass (RenderingParameters::debug_markers gates it).
     * Vertex slot is statically reserved (physics_debug_point_ptr), the
     * 16 line-strip points are rewritten every frame. */
    void update_probe_marker(Proxy& p)
    {
        DebugMarkers* markers;
        p.subsystem(markers);
        if(!markers->available())
            return;

        if(!m_marker_spawned)
        {
            compo::EntityRecipe marker;
            marker.components = {compo::type_hash_v<DebugDraw>()};
            auto       ent    = p.create_entity(marker);
            DebugDraw& draw   = ent.template get<DebugDraw>();
            draw.data.arrays  = {
                 .count  = 16,
                 .offset = physics_debug_point_ptr,
            };
            draw.color_ptr   = physics_debug_color_ptr;
            m_marker_spawned = true;
        }

        auto const& org = m_probe_body->getWorldTransform().getOrigin();
        Vecf3       lo  = Vecf3{org.x(), org.y(), org.z()} - probe_radius;
        Vecf3       hi  = Vecf3{org.x(), org.y(), org.z()} + probe_radius;

        auto const box = DebugMarkers::box_vertices(lo, hi);
        markers->map();
        markers->put_strip(
            physics_debug_point_ptr,
            physics_debug_color_ptr,
            box,
            Vecf3{1.f, .5f, 0.f});
        markers->unmap();
    }

    BSPItem const* find_section_item(BSPCache<V>& cache) const
    {
        for(auto& [id, candidate] : cache.m_cache)
            if(candidate.valid() &&
               candidate.section_idx == cache.active_section &&
               !candidate.coll_surfaces.empty())
                return &candidate;
        return nullptr;
    }

    void rebuild_world(BSPCache<V>& cache)
    {
        Coffee::ProfContext _("Physics::rebuild_world()");
        m_built_section = cache.active_section;

        BSPItem const* item = find_section_item(cache);

        /* Tear down previous static body */
        if(m_world_body)
        {
            m_world->removeRigidBody(m_world_body.get());
            m_world_body.reset();
        }
        if(m_probe_body)
        {
            m_world->removeRigidBody(m_probe_body.get());
            m_probe_body.reset();
        }
        m_world_shape.reset();
        m_mesh_iface.reset();
        m_triangles.clear();
        m_tri_surface.clear();

        if(!item)
        {
            Coffee::Profiler::Profile("Physics::rebuild_world() no collision mesh");
            cDebug(
                "physics: no collision mesh for section {}",
                m_built_section);
            return;
        }

        auto t0 = steady_clock::now();
        triangulate(*item);
        auto t1 = steady_clock::now();

        btIndexedMesh mesh;
        mesh.m_numTriangles        = static_cast<int>(m_triangles.size() / 3);
        mesh.m_triangleIndexBase   = reinterpret_cast<unsigned char const*>(
            m_triangles.data());
        mesh.m_triangleIndexStride = 3 * sizeof(i32);
        mesh.m_numVertices = static_cast<int>(item->coll_vertices.size());
        /* Zero-copy: vertex positions read in place from the mapped tag
         * data; collision::vertex is {Vecf3 point; i32 first_edge} = 16 B */
        mesh.m_vertexBase = reinterpret_cast<unsigned char const*>(
            item->coll_vertices.data());
        mesh.m_vertexStride = sizeof(blam::collision::vertex);
        mesh.m_vertexType   = PHY_FLOAT;

        m_mesh_iface = std::make_unique<btTriangleIndexVertexArray>();
        m_mesh_iface->addIndexedMesh(mesh, PHY_INTEGER);

        auto t2 = steady_clock::now();
        m_world_shape = std::make_unique<btBvhTriangleMeshShape>(
            m_mesh_iface.get(), true /* compress BVH */);
        auto t3 = steady_clock::now();

        btRigidBody::btRigidBodyConstructionInfo info(
            0.f, nullptr, m_world_shape.get());
        info.m_startWorldTransform.setIdentity();
        m_world_body = std::make_unique<btRigidBody>(info);
        m_world_body->setFriction(1.f);
        m_world->addRigidBody(m_world_body.get());

        cDebug(
            "physics: section {} baked: {} surfaces -> {} tris, "
            "{} verts (zero-copy); triangulate {} us, BVH build {} us",
            m_built_section,
            item->coll_surfaces.size(),
            m_triangles.size() / 3,
            item->coll_vertices.size(),
            duration_cast<microseconds>(t1 - t0).count(),
            duration_cast<microseconds>(t3 - t2).count());

        validate_raycasts(*item);
        spawn_probe(*item);
        m_frame = 0;
    }

    template<typename IType, Physics::Event::type_t Type>
    auto create_body(Physics::BodyCreation<IType, Type> const& body_create)
    {
        cDebug("create_body({})", body_create.entity_id);
        entity_body& entity_body = m_bodies[body_create.entity_id];

        if(entity_body.world_body)
        {
            m_world->removeRigidBody(entity_body.world_body.get());
            entity_body.world_body.reset();
            entity_body.world_shape.reset();
            entity_body.mesh_iface.reset();
        }

        btIndexedMesh mesh;
        mesh.m_numTriangles = body_create.indices.size() / 3;
        mesh.m_triangleIndexStride = sizeof(IType) * 3;
        mesh.m_indexType = []()
        {
            if constexpr(std::is_same_v<IType, u16>)
                return PHY_SHORT;
            if constexpr(std::is_same_v<IType, u32>)
                return PHY_INTEGER;
        }();
        mesh.m_numVertices = body_create.vertices.size_bytes() / body_create.vertex_stride;
        mesh.m_vertexStride = body_create.vertex_stride;
        mesh.m_vertexType = [&body_create]()
        {
            switch(body_create.vertex_type)
            {
            default:
                return PHY_FLOAT;
            }
        }();

        mesh.m_vertexBase = reinterpret_cast<unsigned char const*>(
            body_create.vertices.data());
        mesh.m_triangleIndexBase = reinterpret_cast<unsigned char const*>(
            body_create.indices.data());

        entity_body.mesh_iface = std::make_unique<btTriangleIndexVertexArray>();
        entity_body.mesh_iface->addIndexedMesh(mesh, mesh.m_indexType);
        entity_body.world_shape = std::make_unique<btBvhTriangleMeshShape>(
            entity_body.mesh_iface.get(),
            true);
        btRigidBody::btRigidBodyConstructionInfo info{
            body_create.mass,
            nullptr,
            entity_body.world_shape.get(),
        };
        entity_body.world_body = std::make_unique<btRigidBody>(info);
        entity_body.world_body->setFriction(1.f);
        m_world->addRigidBody(entity_body.world_body.get());
    }

    /* Create body with pre-defined shape
     */
    auto create_body(Physics::BodyCreationShape const& body_create)
    {
        cDebug("Spawning {} shape at {} for entity {} (had_body={})",
            magic_enum::enum_name(body_create.shape),
            body_create.position,
            body_create.entity_id,
            m_bodies.contains(body_create.entity_id) &&
                m_bodies[body_create.entity_id].world_body != nullptr);
        entity_body& entity_body = m_bodies[body_create.entity_id];

        /* Same teardown as the mesh path above: the previous body must
         * leave the world before its unique_ptr is overwritten, otherwise
         * the broadphase keeps a dangling pointer and the next
         * stepSimulation() crashes. */
        if(entity_body.world_body)
        {
            m_world->removeRigidBody(entity_body.world_body.get());
            entity_body.world_body.reset();
            entity_body.world_shape.reset();
            entity_body.mesh_iface.reset();
        }

        switch(body_create.shape)
        {
        case Physics::BodyCreationShape::Capsule:
        {
            auto capsule = std::make_unique<btCapsuleShape>(
                body_create.scale.x, 
                body_create.scale.z);
            entity_body.world_shape = std::move(capsule);
            break;
        }
        case Physics::BodyCreationShape::Sphere:
        {
            entity_body.world_shape =
                std::make_unique<btSphereShape>(body_create.scale.x);
            break;
        }
        case Physics::BodyCreationShape::Box:
        {
            entity_body.world_shape = std::make_unique<btBoxShape>(btVector3(
                body_create.scale.x,
                body_create.scale.y,
                body_create.scale.z));
            break;
        }
        }
        if(!entity_body.world_shape)
        {
            cWarning(
                "BodyCreationShape: unhandled shape {}, refusing to create "
                "a body with a null collision shape",
                magic_enum::enum_name(body_create.shape));
            return;
        }
        btVector3 local_inertia(0, 0, 0);
        if(body_create.mass > 0.f)
            entity_body.world_shape->calculateLocalInertia(
                body_create.mass, local_inertia);

        btRigidBody::btRigidBodyConstructionInfo info{
            body_create.mass,
            nullptr,
            entity_body.world_shape.get(),
            local_inertia,
        };
        entity_body.world_body = std::make_unique<btRigidBody>(info);
        if(body_create.lock.rotation)
        {
            /* Upright biped: friction against the triangle soup makes the
             * capsule catch on surface seams and slopes when driven
             * horizontally. Its velocity is authored per-frame anyway, so
             * drop friction entirely — stopping is done by writing zero
             * horizontal velocity, not by friction. */
            entity_body.world_body->setFriction(0.f);
            entity_body.world_body->setAngularFactor(btVector3(0, 0, 0));
        } else
            entity_body.world_body->setFriction(1.f);
        btTransform transform = m_world_basis;
        transform.setOrigin(btVector3(
            body_create.position.x,
            body_create.position.y,
            body_create.position.z));
        entity_body.world_body->setWorldTransform(transform);
        for(int i = 0; i < m_world->getNumCollisionObjects(); ++i)
            if(m_world->getCollisionObjectArray()[i] ==
               entity_body.world_body.get())
                cWarning(
                    "create_body(shape): fresh body {:p} for entity {} "
                    "ALREADY in world at index {} ({} objects total)",
                    (void*)entity_body.world_body.get(),
                    body_create.entity_id,
                    i,
                    m_world->getNumCollisionObjects());
        m_world->addRigidBody(entity_body.world_body.get());
    }

    /* Sleeping bodies are not woken when the static mesh under them is
     * swapped — a resting probe would freeze mid-air over new terrain.
     * Wake everything after any static-world change. */
    void wake_dynamic_bodies()
    {
        auto& bodies = m_world->getNonStaticRigidBodies();
        for(int i = 0; i < bodies.size(); ++i)
            bodies[i]->activate(true);
    }

    /* Adopt a shape built on a worker thread (see BodyCreationPrebuilt) —
     * main-thread cost is one rigid-body alloc + broadphase insert. */
    void adopt_body(Physics::BodyCreationPrebuilt&& body_create)
    {
        remove_body(body_create.entity_id);

        entity_body& body = m_bodies[body_create.entity_id];
        body.mesh_iface   = std::move(body_create.mesh_iface);
        body.world_shape  = std::move(body_create.shape);
        body.keep_alive   = std::move(body_create.keep_alive);

        btRigidBody::btRigidBodyConstructionInfo info{
            0.f,
            nullptr,
            body.world_shape.get(),
        };
        body.world_body = std::make_unique<btRigidBody>(info);
        body.world_body->setFriction(1.f);
        m_world->addRigidBody(body.world_body.get());
        wake_dynamic_bodies();
    }

    void remove_body(u64 entity_id)
    {
        auto it = m_bodies.find(entity_id);
        if(it == m_bodies.end())
            return;
        if(it->second.world_body)
            m_world->removeRigidBody(it->second.world_body.get());
        m_bodies.erase(it);
        wake_dynamic_bodies();
    }

    void add_impulse(Physics::Impulse const& impulse)
    {
        auto body_it = m_bodies.find(impulse.entity_id);
        if(body_it == m_bodies.end())
            return;
        cDebug("Applying impulse of {} to {}", impulse.impulse, impulse.entity_id);
        entity_body& body = (*body_it).second;
        body.world_body->activate(true);
        body.world_body->applyCentralImpulse(btVector3(
            impulse.impulse.x,
            impulse.impulse.y,
            impulse.impulse.z
        ));
    }

    void set_linear_velocity(Physics::Velocity const& velocity)
    {
        auto body_it = m_bodies.find(velocity.entity_id);
        if(body_it == m_bodies.end())
            return;
        entity_body& body = (*body_it).second;
        /* No cDebug here: biped movement sends this every frame */
        btVector3 vel(
            velocity.velocity.x, velocity.velocity.y, velocity.velocity.z);
        if(velocity.preserve_z)
            vel.setZ(body.world_body->getLinearVelocity().z());
        if(velocity.jump != 0.f &&
           std::abs(body.world_body->getLinearVelocity().z()) < 0.1f)
            vel.setZ(velocity.jump);
        body.world_body->activate(true); /* sleeping bodies ignore velocity */
        body.world_body->setLinearVelocity(vel);
    }

    void translate(Physics::Translate const& translation)
    {
        cDebug("Applying translation of {} to {}",
            translation.position,
            translation.entity_id);
        auto body_it = m_bodies.find(translation.entity_id);
        if(body_it == m_bodies.end())
            return;
        entity_body& body = (*body_it).second;
        body.world_body->activate(true);
        btTransform transform = m_world_basis;
        transform.setOrigin(btVector3(
            translation.position.x,
            translation.position.y,
            translation.position.z
        ));
        if(!translation.preserve_momentum)
            body.world_body->setLinearVelocity(btVector3());
        body.world_body->setWorldTransform(transform);
    }

    /* Winged-edge polygon walk: starting at first_edge, follow
     * forward_edge while this surface is on the edge's left, otherwise
     * reverse_edge; collect vertices in winding order, then fan. */
    void triangulate(BSPItem const& item)
    {
        Coffee::ProfContext _("Physics::triangulate()");
        auto const& surfaces = item.coll_surfaces;
        auto const& edges    = item.coll_edges;

        std::vector<i32> polygon;
        for(u32 si = 0; si < surfaces.size(); si++)
        {
            polygon.clear();
            i32 const first = surfaces[si].first_edge;
            i32       e     = first;
            u32       guard = 0;
            do
            {
                if(e < 0 || static_cast<u32>(e) >= edges.size())
                    break;
                auto const& edge = edges[e];
                if(edge.left_surface == static_cast<i32>(si))
                {
                    polygon.push_back(edge.start_vertex);
                    e = edge.forward_edge;
                } else
                {
                    polygon.push_back(edge.end_vertex);
                    e = edge.reverse_edge;
                }
            } while(e != first && ++guard < 32);

            for(u32 i = 2; i < polygon.size(); i++)
            {
                m_triangles.push_back(polygon[0]);
                m_triangles.push_back(polygon[i - 1]);
                m_triangles.push_back(polygon[i]);
                m_tri_surface.push_back(si);
            }
        }
    }

    /* Grid of downward rays across the section bounds; every ray is
     * traced through both the Bullet BVH and the native BSP walk, hit
     * distances compared and both paths timed. */
    void validate_raycasts(BSPItem const& item)
    {
        auto [bmin, bmax] = item.mesh->world_bounds.points();

        constexpr u32 grid    = 16;
        f32 const     z_start = bmax.z + 1.f;
        f32 const     z_end   = bmin.z - 1.f;

        u32 bullet_hits = 0, native_hits = 0, matched = 0, unmatched = 0;
        steady_clock::duration bullet_time{}, native_time{};
        f32 max_dev = 0.f;

        for(u32 gy = 0; gy < grid; gy++)
            for(u32 gx = 0; gx < grid; gx++)
            {
                f32 x = bmin.x + (bmax.x - bmin.x) * (gx + .5f) / grid;
                f32 y = bmin.y + (bmax.y - bmin.y) * (gy + .5f) / grid;

                Vecf3 from{x, y, z_start};
                Vecf3 to{x, y, z_end};

                /* The ray starts outside the sealed world; Bullet reports
                 * every triangle crossing (including the invisible
                 * sky-seal faces), the native trace skips leading solid
                 * and reports the first empty→solid boundary. The native
                 * t must therefore coincide with *one of* Bullet's hit
                 * fractions. */
                auto b0 = steady_clock::now();
                btCollisionWorld::AllHitsRayResultCallback cb(
                    btVector3(from.x, from.y, from.z),
                    btVector3(to.x, to.y, to.z));
                m_world->rayTest(cb.m_rayFromWorld, cb.m_rayToWorld, cb);
                bullet_time += steady_clock::now() - b0;

                auto n0  = steady_clock::now();
                auto hit = item.raycast(from, to);
                native_time += steady_clock::now() - n0;

                if(cb.hasHit())
                    bullet_hits++;
                if(hit)
                {
                    native_hits++;
                    f32 best = std::numeric_limits<f32>::max();
                    for(int i = 0; i < cb.m_hitFractions.size(); i++)
                        best = std::min(
                            best, std::abs(cb.m_hitFractions[i] - hit->t));
                    if(best < 0.005f)
                    {
                        matched++;
                        max_dev = std::max(max_dev, best);
                    } else
                        unmatched++;
                }
            }

        u32 const rays      = grid * grid;
        auto      bullet_us = duration_cast<microseconds>(bullet_time).count();
        auto      native_us = duration_cast<microseconds>(native_time).count();
        cDebug(
            "physics: raycast cross-check, {} rays: bullet {} hits "
            "({} us, {:.2f} us/ray), native {} hits ({} us, {:.2f} us/ray); "
            "{} native hits matched a bullet fraction, {} unmatched, "
            "max matched deviation {:.5f}",
            rays,
            bullet_hits,
            bullet_us,
            f32(bullet_us) / rays,
            native_hits,
            native_us,
            f32(native_us) / rays,
            matched,
            unmatched,
            max_dev);
    }

    /* Dynamic-body smoke test: drop a sphere onto the world mesh and let
     * start_restricted() log it settling. */
    void spawn_probe(BSPItem const& item)
    {
        auto [bmin, bmax] = item.mesh->world_bounds.points();
        Vecf3 center      = (bmin + bmax) * .5f;
        spawn_probe_at(Vecf3{center.x, center.y, bmax.z + .5f});
    }

    /* Create the debug probe sphere at an absolute position — used both by
     * the Halo BSP path (drop from section bounds) and RS2-only sessions,
     * which have no BSPItem to derive a spawn point from. */
    void spawn_probe_at(Vecf3 const& pos)
    {
        if(m_probe_body)
        {
            m_world->removeRigidBody(m_probe_body.get());
            m_probe_body.reset();
        }
        if(!m_probe_shape)
            m_probe_shape = std::make_unique<btSphereShape>(probe_radius);
        btVector3 inertia;
        m_probe_shape->calculateLocalInertia(10.f, inertia);
        btRigidBody::btRigidBodyConstructionInfo info(
            10.f, nullptr, m_probe_shape.get(), inertia);
        info.m_startWorldTransform.setIdentity();
        info.m_startWorldTransform.setOrigin(
            btVector3(pos.x, pos.y, pos.z));
        m_probe_body = std::make_unique<btRigidBody>(info);
        m_probe_body->setActivationState(DISABLE_DEACTIVATION);
        m_probe_body->setFriction(1.f);
        m_probe_body->setRollingFriction(.3f);
        m_probe_body->setSpinningFriction(.3f);
        /* Continuous collision: the RS2 world is scaled down (world_scale)
         * so terrain tiles are tiny and the triangle mesh is infinitely
         * thin, while gravity stays at Halo scale — the sphere accelerates
         * fast enough to tunnel through the surface between substeps. Swept-
         * sphere CCD sweeps the motion each step so it can't pass through. */
        m_probe_body->setCcdMotionThreshold(probe_radius * .5f);
        m_probe_body->setCcdSweptSphereRadius(probe_radius * .8f);
        m_world->addRigidBody(m_probe_body.get());
    }

    void move_probe(Vecf3 const& pos)
    {
        // Recreate the body from scratch rather than teleporting it. A
        // setWorldTransform() teleport leaves the CCD interpolation
        // transform at the old position, so the next step sweeps the probe
        // across the whole map (old→new) and it tunnels / ends up in a bad
        // state — collision then silently fails. The fresh-spawn path is
        // known-good, so reuse it.
        cDebug("(Re)spawning physics probe at {}, {}, {}", pos.x, pos.y, pos.z);
        spawn_probe_at(pos);
    }

    std::unique_ptr<btDefaultCollisionConfiguration>     m_config;
    std::unique_ptr<btCollisionDispatcher>               m_dispatcher;
    std::unique_ptr<btDbvtBroadphase>                    m_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld>             m_world;

    btTransform m_world_basis;

    std::unique_ptr<btTriangleIndexVertexArray> m_mesh_iface;
    std::unique_ptr<btBvhTriangleMeshShape>     m_world_shape;
    std::unique_ptr<btRigidBody>                m_world_body;
    std::unique_ptr<btSphereShape>              m_probe_shape;
    std::unique_ptr<btRigidBody>                m_probe_body;

    struct entity_body
    {
        std::unique_ptr<btTriangleIndexVertexArray> mesh_iface;
        std::unique_ptr<btCollisionShape>           world_shape;
        std::unique_ptr<btRigidBody>                world_body;
        std::shared_ptr<void>                       keep_alive;
    };
    std::map<u64, entity_body> m_bodies;


    /* Index buffer for the world shape + triangle → collision surface
     * mapping (material lookup on hit) */
    std::vector<i32> m_triangles;
    std::vector<u32> m_tri_surface;

    static constexpr f32 probe_radius = 0.2f;

    i16  m_built_section{-2};
    u32  m_frame{0};
    bool m_marker_spawned{false};
};

void alloc_physics(compo::EntityContainer& container)
{
    ProfContext _;
    auto& phys_bus = container.register_subsystem_inplace<PhysicsBus>();
    auto& physics = container.register_subsystem_inplace<PhysicsSystem<halo_version>>();
    phys_bus.addEventFunction<Physics::BodyCreationU16>(
        0, [&physics](Physics::Event&, Physics::BodyCreationU16* body_create)
        {
            physics.create_body(*body_create);
        });
    phys_bus.addEventFunction<Physics::BodyCreationPrebuilt>(
        0,
        [&physics](Physics::Event&, Physics::BodyCreationPrebuilt* body)
        {
            physics.adopt_body(std::move(*body));
        });
    phys_bus.addEventFunction<Physics::BodyCreationShape>(
        0,
        [&physics](Physics::Event&, Physics::BodyCreationShape* body)
        {
            physics.create_body(*body);
        });
    phys_bus.addEventFunction<Physics::BodyRemoval>(
        0, [&physics](Physics::Event&, Physics::BodyRemoval* removal)
        {
            physics.remove_body(removal->entity_id);
        });
    phys_bus.addEventFunction<Physics::Impulse>(
        0, [&physics](Physics::Event&, Physics::Impulse* impulse)
        {
            physics.add_impulse(*impulse);
        });
    phys_bus.addEventFunction<Physics::Velocity>(
        0, [&physics](Physics::Event&, Physics::Velocity* velocity)
        {
            physics.set_linear_velocity(*velocity);
        });
    phys_bus.addEventFunction<Physics::Translate>(
        0, [&physics](Physics::Event&, Physics::Translate* translation)
        {
            physics.translate(*translation);
        });
    phys_bus.addEventFunction<Physics::ProbeHere>(
        0, [&container, &physics](Physics::Event&, Physics::ProbeHere*) {
            for(auto const& e : container.select<PlayerCamera>())
            {
                auto* info = container.get<PlayerInfo>(e.id);
                if(info->seat_idx != 0)
                    continue;
                auto* camera = container.get<PlayerCamera>(e.id);
                physics.move_probe(camera->camera->position);
                break;
            }
        });
}

#else

#include <coffee/core/debug/formatting.h>

void alloc_physics(compo::EntityContainer&)
{
    container.register_subsystem_inplace<PhysicsBus>();
    Coffee::cDebug("physics: built without Bullet support, subsystem disabled");
}

#endif
