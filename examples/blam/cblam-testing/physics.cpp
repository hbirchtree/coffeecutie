#include "physics.h"

#include "caching.h"
#include "coffee/core/CProfiling"
#include "data.h"
#include "map_marker.h"
#include "selected_version.h"

#include <coffee/core/debug/formatting.h>

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
    type_list_t<DebugDraw>,
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

        if(!m_world_body)
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

        m_probe_shape = std::make_unique<btSphereShape>(probe_radius);
        btVector3 inertia;
        m_probe_shape->calculateLocalInertia(10.f, inertia);
        btRigidBody::btRigidBodyConstructionInfo info(
            10.f, nullptr, m_probe_shape.get(), inertia);
        info.m_startWorldTransform.setIdentity();
        info.m_startWorldTransform.setOrigin(
            btVector3(center.x, center.y, bmax.z + .5f));
        m_probe_body = std::make_unique<btRigidBody>(info);
        /* High friction + rolling friction so the sphere parks near its
         * drop point instead of rolling downhill forever */
        m_probe_body->setFriction(1.f);
        m_probe_body->setRollingFriction(.3f);
        m_probe_body->setSpinningFriction(.3f);
        m_world->addRigidBody(m_probe_body.get());
    }

    std::unique_ptr<btDefaultCollisionConfiguration>     m_config;
    std::unique_ptr<btCollisionDispatcher>               m_dispatcher;
    std::unique_ptr<btDbvtBroadphase>                    m_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld>             m_world;

    std::unique_ptr<btTriangleIndexVertexArray> m_mesh_iface;
    std::unique_ptr<btBvhTriangleMeshShape>     m_world_shape;
    std::unique_ptr<btRigidBody>                m_world_body;
    std::unique_ptr<btSphereShape>              m_probe_shape;
    std::unique_ptr<btRigidBody>                m_probe_body;

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
    container.register_subsystem_inplace<PhysicsSystem<halo_version>>();
}

#else

#include <coffee/core/debug/formatting.h>

void alloc_physics(compo::EntityContainer&)
{
    Coffee::cDebug("physics: built without Bullet support, subsystem disabled");
}

#endif
