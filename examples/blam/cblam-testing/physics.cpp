#include "physics.h"

#include "caching.h"
#include "coffee/core/CProfiling"
#include "components.h"
#include "data.h"
#include "map_marker.h"
#include "peripherals/stl/math.h"
#include "selected_version.h"
#include "types.h"

#include <coffee/core/debug/formatting.h>
#include <magic_enum/magic_enum.hpp>

#if defined(FEATURE_ENABLE_BULLET3)

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
#include <btBulletDynamicsCommon.h>

#include <algorithm>
#include <chrono>
#include <limits>
#include <memory>
#include <set>
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
/* A vehicle meets the world only through its mass points: spheres that sink
 * into the ground up to `ground_depth` like springs, hover on antigrav, and
 * grip along or across their forward axis. Halo's fractions are per 30 Hz
 * tick. Spring stiffness and the friction scale are tuning, not tag data. */
class MassPointAction : public btActionInterface
{
  public:
    MassPointAction(
        btRigidBody&                      body,
        std::shared_ptr<MassPoints const> data,
        std::optional<VehicleDrive>       drive)
        : m_body(body)
        , m_data(std::move(data))
        , m_drive(drive)
    {
        for(auto const& point : m_data->points)
        {
            auto sphere = std::make_unique<btSphereShape>(point.radius);
            auto probe  = std::make_unique<btCollisionObject>();
            probe->setCollisionShape(sphere.get());
            m_spheres.push_back(std::move(sphere));
            m_probes.push_back(std::move(probe));
            if(lifts(point))
                m_lifting++;
        }
    }

    /* Held for a few substeps, so a frame rate under the step rate doesn't
     * make the throttle stutter */
    void steer(f32 throttle, btVector3 const& aim)
    {
        m_throttle   = std::clamp(throttle, -1.f, 1.f);
        m_aim        = aim;
        m_input_left = 12;
    }

    void updateAction(btCollisionWorld* world, btScalar dt) override
    {
        if(m_body.isStaticOrKinematicObject() || !m_body.isActive())
            return;
        bool const driven = m_drive && m_input_left > 0;
        if(m_input_left > 0)
            m_input_left--;
        if(!driven)
            m_throttle = 0.f;
        /* Wheels roll while there is a driver giving throttle */
        bool const rolling = driven && std::abs(m_throttle) > .05f;
        MassPoints const& data  = *m_data;
        btTransform const body  = m_body.getWorldTransform();
        btVector3 const   com   = to_bt(data.center_of_mass);
        f32 const         mass  = data.mass;
        f32 const         gravity = m_body.getGravity().length();

        struct touch_t
        {
            size_t    point;
            btVector3 at;
            f32       depth;
            btVector3 normal;
        };
        std::vector<touch_t> touches;
        for(size_t i = 0; i < data.points.size(); i++)
        {
            btVector3 const at = body * (to_bt(data.points[i].position) - com);
            if(auto touch = deepest_contact(world, i, at))
                touches.push_back({i, at, touch->first, touch->second});
        }

        if(!touches.empty() && data.ground_depth > 0.f)
        {
            /* Shared so the vehicle settles half way into ground_depth,
             * however many points touch */
            f32 const count = static_cast<f32>(touches.size());
            f32 const stiffness =
                2.f * mass * gravity / data.ground_depth;
            f32 const damping =
                2.f * ground_damping_ratio * std::sqrt(stiffness * mass);
            for(auto const& touch : touches)
            {
                auto const&     point = data.points[touch.point];
                btVector3 const rel   = touch.at - body.getOrigin();
                btVector3 const v     = m_body.getVelocityInLocalPoint(rel);
                f32 const       vn    = v.dot(touch.normal);
                f32             push =
                    (stiffness * touch.depth - damping * vn) / count;
                if(touch.depth > data.ground_depth)
                    push += 10.f * stiffness *
                            (touch.depth - data.ground_depth) / count;
                push = std::max(push, 0.f);
                m_body.applyImpulse(touch.normal * (push * dt), rel);

                /* The hulls take part of the weight when the springs
                 * bottom out, so grip goes by at least the point's share */
                f32 const load = std::max(push, mass * gravity / count);
                f32 const grip =
                    slope(touch.normal.z(), data.ground_normal_k0,
                          data.ground_normal_k1) *
                    data.ground_friction * friction_scale * load * dt;
                bool const drives = traction(point);
                friction(
                    point,
                    body,
                    touch.normal,
                    v,
                    rel,
                    grip,
                    mass / count,
                    drives && steered(point) ? m_steering : 0.f,
                    drives && rolling);
            }
        }

        if(driven)
            drive(touches, body, mass, dt);

        for(size_t i = 0; i < data.points.size(); i++)
            if(lifts(data.points[i]))
                antigrav(world, i, body, com, mass, gravity, dt);
    }

    void debugDraw(btIDebugDraw*) override
    {
    }

  private:
    template<typename Touches>
    void drive(
        Touches const& touches, btTransform const& body, f32 mass, f32 dt)
    {
        VehicleDrive const& d = *m_drive;
        btVector3 forward     = body.getBasis() * btVector3(1, 0, 0);
        forward.setZ(0.f);
        if(forward.length2() < 1e-6f)
            return;
        forward.normalize();
        btVector3 aim = m_aim;
        aim.setZ(0.f);
        aim = aim.length2() > 1e-6f ? aim.normalized() : forward;
        f32 const heading =
            std::atan2(forward.cross(aim).z(), forward.dot(aim));

        /* Toward the target speed, no harder than the tag allows */
        btVector3 const velocity = m_body.getLinearVelocity();
        f32 const       speed    = velocity.dot(forward);
        f32 const       target   = m_throttle >= 0.f
                                       ? m_throttle * d.forward_speed
                                       : m_throttle * d.reverse_speed;
        bool const speeding_up =
            std::abs(target) > std::abs(speed) && target * speed >= 0.f;
        f32 const limit =
            speeding_up || d.deceleration <= 0.f ? d.acceleration
                                                 : d.deceleration;
        f32 const accel =
            std::clamp((target - speed) / dt, -limit, limit);

        bool const wheeled = d.type == VehicleDrive::human_jeep ||
                             d.type == VehicleDrive::human_tank;
        if(wheeled)
        {
            /* Through the tires or treads touching the ground */
            size_t pushing = 0;
            for(auto const& touch : touches)
                pushing += traction(m_data->points[touch.point]);
            for(auto const& touch : touches)
                if(pushing && traction(m_data->points[touch.point]))
                    m_body.applyImpulse(
                        forward * (mass * accel * dt / pushing),
                        touch.at - body.getOrigin());
        } else
        {
            m_body.applyCentralImpulse(forward * (mass * accel * dt));
            /* Hovercraft would otherwise drift sideways like on ice */
            btVector3 drift = velocity - forward * speed;
            drift.setZ(0.f);
            m_body.applyCentralImpulse(
                drift * (-mass * std::min(1.f, hover_grip * dt)));
        }

        if(d.type == VehicleDrive::human_jeep)
        {
            /* Front wheels turn toward the aim at the tag's rate */
            f32 const want = std::clamp(heading, -d.max_turn, d.max_turn);
            f32 const step = (d.turn_rate > 0.f ? d.turn_rate : 3.f) * dt;
            m_steering += std::clamp(want - m_steering, -step, step);
            return;
        }
        /* Everything else turns its body toward the aim */
        f32 const rate = d.turn_rate > 0.f && d.turn_rate < 10.f
                             ? std::max(d.turn_rate, default_yaw_rate)
                             : default_yaw_rate;
        f32 const want  = std::clamp(heading * 3.f, -rate, rate);
        btVector3 const up = btVector3(0, 0, 1);
        f32 const yaw   = m_body.getAngularVelocity().dot(up);
        f32 const inertia =
            m_data->moments.z > 0.f ? m_data->moments.z : mass;
        m_body.applyTorqueImpulse(
            up * (inertia * (want - yaw) * std::min(1.f, 10.f * dt)));
    }

    bool traction(MassPoints::point_t const& point) const
    {
        return point.powered >= 0 &&
               static_cast<size_t>(point.powered) < m_data->powered.size() &&
               m_data->powered[point.powered].traction;
    }

    /* Front wheels steer */
    bool steered(MassPoints::point_t const& point) const
    {
        return m_drive && m_drive->type == VehicleDrive::human_jeep &&
               point.position.x > m_data->center_of_mass.x;
    }

    static constexpr f32 ground_damping_ratio   = 0.5f;
    static constexpr f32 antigrav_damping_ratio = 0.3f;
    static constexpr f32 friction_scale         = 4.f;
    static constexpr f32 hover_grip             = 3.f;  /* 1/s */
    static constexpr f32 default_yaw_rate       = 1.5f; /* rad/s */

    static btVector3 to_bt(Vecf3 const& v)
    {
        return {v.x, v.y, v.z};
    }

    static f32 slope(f32 normal_z, f32 k0, f32 k1)
    {
        if(k1 <= k0)
            return normal_z >= k1 ? 1.f : 0.f;
        return std::clamp((normal_z - k0) / (k1 - k0), 0.f, 1.f);
    }

    bool lifts(MassPoints::point_t const& point) const
    {
        return point.powered >= 0 &&
               static_cast<size_t>(point.powered) < m_data->powered.size() &&
               m_data->powered[point.powered].antigrav &&
               m_data->powered[point.powered].height > 0.f;
    }

    /* Deepest overlap of point i with the static world: depth and the
     * normal out of the world */
    std::optional<std::pair<f32, btVector3>> deepest_contact(
        btCollisionWorld* world, size_t i, btVector3 const& at)
    {
        struct deepest_t : btCollisionWorld::ContactResultCallback
        {
            btCollisionObject const* self{};
            f32                      depth{0.f};
            btVector3                normal{0, 0, 1};

            btScalar addSingleResult(
                btManifoldPoint&                cp,
                btCollisionObjectWrapper const* a,
                int,
                int,
                btCollisionObjectWrapper const*,
                int,
                int) override
            {
                f32 const d = -cp.getDistance();
                if(d <= depth)
                    return 0;
                depth  = d;
                normal = a->getCollisionObject() == self
                             ? cp.m_normalWorldOnB
                             : -cp.m_normalWorldOnB;
                return 0;
            }
        } deepest;
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(at);
        m_probes[i]->setWorldTransform(transform);
        deepest.self                  = m_probes[i].get();
        deepest.m_collisionFilterGroup = btBroadphaseProxy::DefaultFilter;
        deepest.m_collisionFilterMask  = btBroadphaseProxy::StaticFilter;
        world->contactTest(m_probes[i].get(), deepest);
        if(deepest.depth <= 0.f)
            return std::nullopt;
        return std::pair{deepest.depth, deepest.normal.normalized()};
    }

    /* Coulomb friction, split along the point's friction axis */
    void friction(
        MassPoints::point_t const& point,
        btTransform const&         body,
        btVector3 const&           normal,
        btVector3 const&           v,
        btVector3 const&           rel,
        f32                        grip,
        f32                        share,
        f32                        steering,
        bool                       rolling)
    {
        btVector3 const slip = v - normal * v.dot(normal);
        auto oppose = [&](btVector3 const& component, f32 limit) {
            f32 const speed = component.length();
            if(speed < 1e-5f || limit <= 0.f)
                return;
            f32 const stop = std::min(speed * share, limit);
            m_body.applyImpulse(component * (-stop / speed), rel);
        };
        if(point.friction == MassPoints::point_t::friction_t::point && !rolling)
        {
            oppose(slip, grip * point.parallel);
            return;
        }
        /* A driven tread rolls along the body like a wheel */
        btVector3 axis =
            point.friction == MassPoints::point_t::friction_t::point ||
                    point.friction == MassPoints::point_t::friction_t::forward
                ? to_bt(point.forward)
            : point.friction == MassPoints::point_t::friction_t::up
                ? to_bt(point.up)
                : to_bt(point.up).cross(to_bt(point.forward));
        if(steering != 0.f)
            axis = axis.rotate(to_bt(point.up), steering);
        axis = body.getBasis() * axis;
        axis -= normal * axis.dot(normal);
        if(axis.length2() < 1e-6f)
        {
            oppose(slip, grip * point.perpendicular);
            return;
        }
        axis.normalize();
        btVector3 const along = axis * slip.dot(axis);
        if(!rolling)
            oppose(along, grip * point.parallel);
        oppose(slip - along, grip * point.perpendicular);
    }

    /* Lift that fades out at `height` above the ground, sharing the
     * vehicle's weight between the lifting points */
    void antigrav(
        btCollisionWorld*  world,
        size_t             i,
        btTransform const& body,
        btVector3 const&   com,
        f32                mass,
        f32                gravity,
        f32                dt)
    {
        auto const& point   = m_data->points[i];
        auto const& powered = m_data->powered[point.powered];
        btVector3 const at  = body * (to_bt(point.position) - com);
        btVector3 const from =
            at + btVector3(0, 0, -powered.offset);
        btVector3 const to = from - btVector3(0, 0, powered.height);
        btCollisionWorld::ClosestRayResultCallback ground(from, to);
        ground.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
        world->rayTest(from, to, ground);
        if(!ground.hasHit())
            return;
        f32 const height    = ground.m_closestHitFraction * powered.height;
        f32 const share     = mass / static_cast<f32>(m_lifting);
        f32 const stiffness = powered.strength * share * gravity /
                              powered.height;
        btVector3 const rel = at - body.getOrigin();
        f32 const       vz  = m_body.getVelocityInLocalPoint(rel).z();
        f32 lift = stiffness * (powered.height - height) -
                   2.f * antigrav_damping_ratio *
                       std::sqrt(stiffness * share) * vz;
        lift *= slope(
            ground.m_hitNormalWorld.z(), powered.normal_k0, powered.normal_k1);
        if(lift > 0.f)
            m_body.applyImpulse(btVector3(0, 0, lift * dt), rel);
    }

    btRigidBody&                                    m_body;
    std::shared_ptr<MassPoints const>               m_data;
    std::vector<std::unique_ptr<btSphereShape>>     m_spheres;
    std::vector<std::unique_ptr<btCollisionObject>> m_probes;
    size_t                                          m_lifting{0};
    std::optional<VehicleDrive>                     m_drive;
    f32                                             m_throttle{0.f};
    btVector3                                       m_aim{1, 0, 0};
    u32                                             m_input_left{0};
    f32                                             m_steering{0.f};
};

template<typename V>
using PhysicsManifest = compo::SubsystemManifest<
    type_list_t<
        DebugDraw,
        Model,
        ObjectPhysics,
        PhysicsData,
        PlayerCamera,
        const PlayerInfo,
        const NetworkInfo>,
    type_list_t<const BSPCache<V>, DebugMarkers, const LoadingStatus>,
    empty_list_t>;

template<typename V>
struct PhysicsSystem
    : compo::RestrictedSubsystem<PhysicsSystem<V>, PhysicsManifest<V>>
{
    using type  = PhysicsSystem<V>;
    using Proxy = compo::proxy_of<PhysicsManifest<V>>;

    PhysicsSystem()
    {
        // Ordering constraint: before Occluder and DrawListBuilder
        this->priority = 897;
        m_config       = std::make_unique<btDefaultCollisionConfiguration>();
        m_dispatcher = std::make_unique<btCollisionDispatcher>(m_config.get());
        m_broadphase = std::make_unique<btDbvtBroadphase>();
        m_solver     = std::make_unique<btSequentialImpulseConstraintSolver>();
        m_world      = std::make_unique<btDiscreteDynamicsWorld>(
            m_dispatcher.get(),
            m_broadphase.get(),
            m_solver.get(),
            m_config.get());
        /* Halo's gravity: 9.81 m/s^2 with a world unit of 10 ft */
        m_world->setGravity(btVector3(0, 0, -halo_gravity));
        m_world_basis.setIdentity();
        m_world_basis.setRotation(
            btQuaternion(btVector3(1, 0, 0), stl_types::math::pi / 2));
    }

    void start_restricted(Proxy& p, compo::time_point const& t)
    {
        using namespace std::chrono_literals;

        BSPCache<V> const*   bsp_cache;
        LoadingStatus const* loading;
        p.subsystem(bsp_cache);
        p.subsystem(loading);

        if(loading->loading)
            return;

        if(std::exchange(m_reset_holders, false))
            for(auto holder : p.template select<PhysicsData>())
                holder.template get<PhysicsData>().grabbed = 0;

        /* Rebuild on section change, but also retry while no world mesh
         * exists yet — the BSP cache is populated asynchronously after the
         * subsystem starts, and single-BSP maps never switch sections. */
        bool needs_rebuild = bsp_cache->active_section != m_built_section;
        if(!needs_rebuild && !m_world_body)
            needs_rebuild = find_section_item(*bsp_cache) != nullptr;
        if(needs_rebuild)
            rebuild_world(*bsp_cache);

        /* Without ground, new bodies would fall through the map */
        if(m_world_body)
        {
            reconcile_player_bodies(p);
            reconcile_object_bodies(p);
        }

        // Simulate when there is anything in the world: the Halo BSP body,
        // streamed RS2 region bodies, or the debug probe. Gating solely on
        // m_world_body froze RS2-only sessions (no Halo map) — no step and
        // no probe/debug marker update.
        if(!m_world_body && m_bodies.empty() && !m_probe_body)
            return;

        if(m_next_process_time > t)
            return;

        carry_grabbed(p);
        m_world->stepSimulation(step_seconds, 4, step_seconds / 2);
        m_next_process_time = t + 16ms;

        /* Sensor overlaps (trigger volumes): narrowphase still generates
         * contact manifolds for CF_NO_CONTACT_RESPONSE bodies, the solver
         * just skips them — report any touching pair involving a sensor.
         * Entity ids ride on btCollisionObject::userPointer; bodies
         * without one (world mesh, probe) are ignored. */
        if(m_bus)
        {
            auto* dispatcher = m_world->getDispatcher();
            for(int i = 0; i < dispatcher->getNumManifolds(); ++i)
            {
                btPersistentManifold* manifold =
                    dispatcher->getManifoldByIndexInternal(i);
                if(manifold->getNumContacts() == 0)
                    continue;
                btCollisionObject const* a = manifold->getBody0();
                btCollisionObject const* b = manifold->getBody1();
                if(((a->getCollisionFlags() | b->getCollisionFlags()) &
                    btCollisionObject::CF_NO_CONTACT_RESPONSE) == 0)
                    continue;
                u64 id_a =
                    u64(reinterpret_cast<uintptr_t>(a->getUserPointer()));
                u64 id_b =
                    u64(reinterpret_cast<uintptr_t>(b->getUserPointer()));
                if(!id_a || !id_b)
                    continue;
                Physics::Event   event{Physics::Event::Overlap};
                Physics::Overlap overlap{id_a, id_b};
                m_bus->process(event, &overlap);
            }
        }

        if(m_probe_body)
            update_probe_marker(p);

        if(m_probe_body && m_frame < 600 && (m_frame % 60) == 0)
        {
            auto const& org = m_probe_body->getWorldTransform().getOrigin();
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
            m_markers = markers;
            markers->map();
            for(auto& [entity, body] : m_bodies)
            {
                DebugDraw* debug = p.template get<DebugDraw>(entity);
                if(!debug)
                    continue;
                if(!body.debug_slot.valid())
                {
                    body.debug_slot = markers->acquire_strip(16);
                    if(!body.debug_slot.valid())
                        continue;
                    debug->data.arrays = {
                        .count  = body.debug_slot.vert_count,
                        .offset = body.debug_slot.vert_offset,
                    };
                    debug->color_ptr = body.debug_slot.color_idx;
                }
                btRigidBody* rigid_body = body.world_body.get();
                btVector3    min, max;
                rigid_body->getAabb(min, max);
                auto const box = DebugMarkers::box_vertices(
                    {min.x(), min.y(), min.z()}, {max.x(), max.y(), max.z()});
                markers->put_strip(
                    body.debug_slot.vert_offset,
                    body.debug_slot.color_idx,
                    box,
                    Vecf3{1.f, .5f, 0.f});
            }
            markers->unmap();
        } while(false);

        for(auto const& player_ : p.template select<PlayerCamera>())
        {
            auto          player = player_;
            PlayerCamera& camera = player.template get<PlayerCamera>();
            auto          info   = p.template get<PlayerInfo>(player.id());
            if(!info || !info->mode.physics || info->is_remote())
                continue;
            auto phys_it = m_bodies.find(player.id());
            if(phys_it == m_bodies.end())
                continue;
            entity_body& phys = (*phys_it).second;
            btVector3&   origin =
                phys.world_body->getWorldTransform().getOrigin();
            camera.camera.position = {
                origin.x(),
                origin.y(),
                origin.z() + info->biped.eye_offset(),
            };
        }

        for(auto const& [entity, _] : m_player_bodies)
        {
            PhysicsData* data = p.template get<PhysicsData>(entity);
            auto         it   = m_bodies.find(entity);
            if(!data || it == m_bodies.end())
                continue;
            auto const& origin =
                it->second.world_body->getWorldTransform().getOrigin();
            data->position = {origin.x(), origin.y(), origin.z()};
        }

        m_frame++;
    }

    /* Which way a biped under this camera faces, about +Z */
    static f32 camera_yaw(PlayerCamera const& camera)
    {
        Vecf3 const forward = glm::transpose(Matf3(camera.rotation)) *
                              Vecf3{0.f, 0.f, -1.f};
        return std::atan2(forward.y, forward.x);
    }

    /* Bodies follow biped_in_play(). A local seat in physics mode drives
     * its camera with a dynamic body; anyone else gets a kinematic one that
     * follows their camera, so collisions happen where bipeds are drawn. */
    void reconcile_player_bodies(Proxy& p)
    {
        std::set<u64> live;
        for(auto player : p.template select<
                          PlayerCamera,
                          PlayerInfo,
                          NetworkInfo,
                          PhysicsData>())
        {
            auto [camera, info, net, data] = player.components();
            if(!biped_in_play(info, camera, net))
                continue;
            u64 const  id        = player.id();
            bool const kinematic = info.is_remote() || !info.mode.physics;
            auto const shape     = info.biped;
            live.insert(id);

            auto existing = m_player_bodies.find(id);
            if(existing == m_player_bodies.end() ||
               existing->second.kinematic != kinematic ||
               existing->second.shape != shape || !m_bodies.contains(id))
            {
                Vecf3 const origin = kinematic
                                         ? camera.camera.position -
                                               Vecf3{0, 0, shape.eye_offset()}
                                         : camera.camera.position +
                                               Vecf3{0, 0, shape.spawn_lift()};
                /* Bodies following a camera are what others hit, so they
                 * get the coll model; a body the player moves with keeps the
                 * capsule the biped tag sizes for movement */
                bool const hulls = kinematic && shape.collision;
                create_body(Physics::BodyCreationShape{
                    .entity_id = id,
                    /* The capsule's height is its cylinder, between the caps */
                    .scale =
                        {shape.radius,
                         0,
                         std::max(shape.height - 2 * shape.radius, 0.f)},
                    .position  = origin,
                    .mass      = kinematic ? 0.f : 1.f,
                    .shape     = hulls ? Physics::BodyCreationShape::Hulls
                                       : Physics::BodyCreationShape::Capsule,
                    .group     = Physics::BodyCreationShape::Character,
                    .kinematic = kinematic,
                    .lock      = {.rotation = true},
                    .hulls     = shape.collision,
                    /* Hull points are from the feet; the origin is mid-body */
                    .offset    = {0, 0, -shape.height / 2},
                    .yaw       = camera_yaw(camera),
                });
                m_player_bodies[id] = {.kinematic = kinematic, .shape = shape};
                data.physics_id     = id;
                data.enabled        = true;
                data.kinematic      = kinematic;
                data.position       = origin;
            } else if(kinematic)
                move_kinematic(
                    id,
                    camera.camera.position - Vecf3{0, 0, shape.eye_offset()},
                    shape.collision ? std::optional(camera_yaw(camera))
                                    : std::nullopt);
        }

        for(auto it = m_player_bodies.begin(); it != m_player_bodies.end();)
        {
            if(live.contains(it->first))
            {
                ++it;
                continue;
            }
            remove_body(it->first);
            if(PhysicsData* data = p.template get<PhysicsData>(it->first))
                data->enabled = data->kinematic = false;
            it = m_player_bodies.erase(it);
        }
    }

    /* World objects with an ObjectPhysics get one body each. Simulated
     * ones carry their Model along; followers are carried by it. */
    void reconcile_object_bodies(Proxy& p)
    {
        using Shape = Physics::BodyCreationShape;
        std::set<u64> live;
        for(auto object : p.template select<Model, ObjectPhysics>())
        {
            auto [model, physics] = object.components();
            u64 const  id         = object.id();
            bool const moves      = physics.mass > 0.f;
            bool const follower =
                moves && physics.authority == ObjectPhysics::Follower;
            live.insert(id);
            /* Bodies with a phys tag turn about its centre of mass */
            Vecf3 const com = physics.mass_points
                                  ? physics.mass_points->center_of_mass
                                  : Vecf3{};
            auto existing = m_object_bodies.find(id);
            if(existing == m_object_bodies.end() ||
               existing->second != physics.authority)
            {
                m_object_bodies[id]  = physics.authority;
                Shape::shape_t shape =
                    physics.collision ? (moves ? Shape::Hulls : Shape::Mesh)
                    : physics.radius > 0.f ? Shape::Sphere
                                           : Shape::Box;
                create_body(Shape{
                    .entity_id = id,
                    .scale     = shape == Shape::Sphere
                                     ? Vecf3(physics.radius)
                                     : physics.half_extents,
                    .position  = model.position + model.rotation * com,
                    .mass      = follower ? 0.f : physics.mass,
                    .shape     = shape,
                    .group     = physics.item          ? Shape::Item
                                 : physics.mass_points ? Shape::Grounded
                                 : moves               ? Shape::Vehicle
                                                       : Shape::AnyGroup,
                    .kinematic = follower,
                    .hulls     = physics.collision,
                    .offset    = physics.mass_points ? -com : physics.center,
                    .rotation  = model.rotation,
                    .mass_points = physics.mass_points,
                    .drive       = physics.drive,
                });
                continue;
            }
            if(!moves)
                continue;
            auto it = m_bodies.find(id);
            if(it == m_bodies.end())
                continue;
            btRigidBody& body = *it->second.world_body;
            if(follower)
            {
                btTransform transform;
                transform.setRotation(btQuaternion(
                    model.rotation.x,
                    model.rotation.y,
                    model.rotation.z,
                    model.rotation.w));
                Vecf3 const origin = model.position + model.rotation * com;
                transform.setOrigin(btVector3(origin.x, origin.y, origin.z));
                body.setWorldTransform(transform);
                continue;
            }
            physics.sleeping = !body.isActive();
            if(physics.sleeping)
            {
                physics.linear_velocity = physics.angular_velocity = {};
                continue;
            }
            auto to_vec = [](btVector3 const& v) {
                return Vecf3{v.x(), v.y(), v.z()};
            };
            physics.linear_velocity  = to_vec(body.getLinearVelocity());
            physics.angular_velocity = to_vec(body.getAngularVelocity());
            btTransform const& transform = body.getWorldTransform();
            btQuaternion const rotation  = transform.getRotation();
            model.rotation =
                Quatf(rotation.w(), rotation.x(), rotation.y(), rotation.z());
            model.position =
                to_vec(transform.getOrigin()) - model.rotation * com;
            model.update_matrix();
        }

        for(auto it = m_object_bodies.begin(); it != m_object_bodies.end();)
        {
            if(live.contains(it->first))
            {
                ++it;
                continue;
            }
            remove_body(it->first);
            it = m_object_bodies.erase(it);
        }
    }

    static btTransform camera_transform(Physics::Grab const& grab)
    {
        Quatf const& to_world = grab.to_world;
        btTransform  transform;
        transform.setRotation(
            btQuaternion(to_world.x, to_world.y, to_world.z, to_world.w));
        transform.setOrigin(
            btVector3(grab.origin.x, grab.origin.y, grab.origin.z));
        return transform;
    }

    /* Swap a body between simulated and carried: Bullet wants it out of the
     * world while its mass and flags change */
    void set_carried(u64 entity_id, bool carried, f32 mass)
    {
        auto&        body  = m_bodies.at(entity_id);
        btRigidBody& rigid = *body.world_body;
        m_world->removeRigidBody(&rigid);
        btVector3 inertia(0, 0, 0);
        if(carried)
        {
            rigid.setMassProps(0.f, inertia);
            rigid.setCollisionFlags(
                rigid.getCollisionFlags() |
                btCollisionObject::CF_KINEMATIC_OBJECT);
            rigid.setActivationState(DISABLE_DEACTIVATION);
        } else
        {
            if(body.mass_points)
                inertia = btVector3(
                    body.mass_points->moments.x,
                    body.mass_points->moments.y,
                    body.mass_points->moments.z);
            else
                body.world_shape->calculateLocalInertia(mass, inertia);
            rigid.setMassProps(mass, inertia);
            rigid.setCollisionFlags(
                rigid.getCollisionFlags() &
                ~btCollisionObject::CF_KINEMATIC_OBJECT);
            /* Held still, it ran up sleep time; don't let it nap mid-air */
            rigid.forceActivationState(ACTIVE_TAG);
            rigid.setDeactivationTime(0.f);
        }
        rigid.updateInertiaTensor();
        add_to_world(rigid, body.group);
    }

    /* The first thing along the view, if it is a moving object this peer
     * simulates */
    std::optional<u64> pick(Proxy& p, Physics::Grab const& grab)
    {
        Vecf3 const     forward  = grab.to_world * Vecf3{0.f, 0.f, -1.f};
        Vecf3 const     end      = grab.origin + forward * grab_range;
        btVector3 const from(grab.origin.x, grab.origin.y, grab.origin.z);
        btVector3 const to(end.x, end.y, end.z);
        btCollisionWorld::AllHitsRayResultCallback hits(from, to);
        /* Items only accept the world and each other; the cursor sees all */
        hits.m_collisionFilterGroup = btBroadphaseProxy::AllFilter;
        m_world->rayTest(from, to, hits);
        std::optional<u64> nearest;
        f32                nearest_t = 2.f;
        for(int i = 0; i < hits.m_collisionObjects.size(); i++)
        {
            u64 const id = u64(reinterpret_cast<uintptr_t>(
                hits.m_collisionObjects[i]->getUserPointer()));
            /* Flycam sits inside its own biped's hulls */
            if(id == grab.entity_id || hits.m_hitFractions[i] >= nearest_t)
                continue;
            nearest_t = hits.m_hitFractions[i];
            nearest   = id;
        }
        if(!nearest)
            return std::nullopt;
        auto const* physics = p.template get<ObjectPhysics>(*nearest);
        if(!physics || physics->mass <= 0.f ||
           physics->authority != ObjectPhysics::Simulated)
            return std::nullopt;
        return nearest;
    }

    /* Holders show what they carry in PhysicsData, and listeners hear
     * when it changes */
    void announce_hold(
        Proxy&           p,
        u64              holder,
        u64              object,
        bool             held,
        btVector3 const& velocity = {0, 0, 0})
    {
        cDebug("physics: {} {} {}", holder, held ? "grabbed" : "dropped", object);
        if(auto* data = p.template get<PhysicsData>(holder))
            data->grabbed = held ? object : 0;
        if(!m_bus)
            return;
        if(held)
        {
            Physics::Event   event{Physics::Event::Grabbed};
            Physics::Grabbed grabbed{.holder = holder, .object = object};
            m_bus->process(event, &grabbed);
            return;
        }
        Physics::Event   event{Physics::Event::Dropped};
        Physics::Dropped dropped{
            .holder   = holder,
            .object   = object,
            .velocity = {velocity.x(), velocity.y(), velocity.z()},
        };
        m_bus->process(event, &dropped);
    }

    void carry_grabbed(Proxy& p)
    {
        for(auto it = m_grab_requests.begin(); it != m_grab_requests.end();)
        {
            Physics::Grab const& grab    = it->second;
            auto                 carried = m_grabs.find(grab.entity_id);
            if(carried != m_grabs.end() &&
               !m_bodies.contains(carried->second.object))
            {
                announce_hold(p, grab.entity_id, carried->second.object, false);
                carried = m_grabs.erase(carried);
            }

            if(grab.held && carried == m_grabs.end())
            {
                if(auto object = pick(p, grab))
                {
                    set_carried(*object, true, 0.f);
                    m_grabs[grab.entity_id] = {
                        .object   = *object,
                        .relative = camera_transform(grab).inverse() *
                                    m_bodies.at(*object)
                                        .world_body->getWorldTransform(),
                    };
                    announce_hold(p, grab.entity_id, *object, true);
                }
            } else if(grab.held)
            {
                btRigidBody& rigid =
                    *m_bodies.at(carried->second.object).world_body;
                btTransform const target =
                    camera_transform(grab) * carried->second.relative;
                carried->second.velocity =
                    (target.getOrigin() -
                     rigid.getWorldTransform().getOrigin()) /
                    step_seconds;
                rigid.setWorldTransform(target);
            } else if(carried != m_grabs.end())
            {
                u64 const   object  = carried->second.object;
                auto const* physics = p.template get<ObjectPhysics>(object);
                set_carried(object, false, physics ? physics->mass : 1.f);
                /* Let go mid-swing, it keeps going */
                m_bodies.at(object).world_body->setLinearVelocity(
                    carried->second.velocity);
                announce_hold(
                    p, grab.entity_id, object, false, carried->second.velocity);
                m_grabs.erase(carried);
            }
            it = grab.held ? std::next(it) : m_grab_requests.erase(it);
        }
    }

    static btTransform yaw_basis(f32 yaw)
    {
        btTransform transform;
        transform.setIdentity();
        transform.setRotation(btQuaternion(btVector3(0, 0, 1), yaw));
        return transform;
    }

    void move_kinematic(
        u64 entity_id, Vecf3 const& position, std::optional<f32> yaw = {})
    {
        auto it = m_bodies.find(entity_id);
        if(it == m_bodies.end())
            return;
        btTransform transform = yaw ? yaw_basis(*yaw) : m_world_basis;
        transform.setOrigin(btVector3(position.x, position.y, position.z));
        it->second.world_body->setWorldTransform(transform);
    }

    /* Wire box following the probe sphere, drawn through the existing
     * debug line pass (RenderingParameters::debug_markers gates it).
     * Vertex slot is reserved once (acquire_strip), the 16 line-strip
     * points are rewritten every frame. */
    void update_probe_marker(Proxy& p)
    {
        DebugMarkers* markers;
        p.subsystem(markers);
        if(!markers->available())
            return;
        m_markers = markers;

        if(!m_marker_spawned)
        {
            m_probe_slot = markers->acquire_strip(16);
            if(!m_probe_slot.valid())
                return;
            compo::EntityRecipe marker;
            marker.components = {compo::type_hash_v<DebugDraw>()};
            auto       ent    = p.create_entity(marker);
            DebugDraw& draw   = ent.template get<DebugDraw>();
            draw.data.arrays  = {
                 .count  = m_probe_slot.vert_count,
                 .offset = m_probe_slot.vert_offset,
            };
            draw.color_ptr   = m_probe_slot.color_idx;
            m_marker_spawned = true;
        }

        auto const& org = m_probe_body->getWorldTransform().getOrigin();
        Vecf3       lo  = Vecf3{org.x(), org.y(), org.z()} - probe_radius;
        Vecf3       hi  = Vecf3{org.x(), org.y(), org.z()} + probe_radius;

        auto const box = DebugMarkers::box_vertices(lo, hi);
        markers->map();
        markers->put_strip(
            m_probe_slot.vert_offset,
            m_probe_slot.color_idx,
            box,
            Vecf3{1.f, .5f, 0.f});
        markers->unmap();
    }

    BSPItem const* find_section_item(BSPCache<V> const& cache) const
    {
        for(auto& [id, candidate] : cache.m_cache)
            if(candidate.valid() &&
               candidate.section_idx == cache.active_section &&
               !candidate.coll_surfaces.empty())
                return &candidate;
        return nullptr;
    }

    void rebuild_world(BSPCache<V> const& cache)
    {
        Coffee::ProfContext _("Physics::rebuild_world()");
        m_built_section = cache.active_section;

        BSPItem const* item = find_section_item(cache);
        m_world_item        = item;

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
            Coffee::Profiler::Profile(
                "Physics::rebuild_world() no collision mesh");
            cDebug(
                "physics: no collision mesh for section {}", m_built_section);
            return;
        }

        auto t0 = steady_clock::now();
        triangulate(*item);
        auto t1 = steady_clock::now();

        btIndexedMesh mesh;
        mesh.m_numTriangles = static_cast<int>(m_triangles.size() / 3);
        mesh.m_triangleIndexBase =
            reinterpret_cast<unsigned char const*>(m_triangles.data());
        mesh.m_triangleIndexStride = 3 * sizeof(i32);
        mesh.m_numVertices = static_cast<int>(item->coll_vertices.size());
        /* Zero-copy: vertex positions read in place from the mapped tag
         * data; collision::vertex is {Vecf3 point; i32 first_edge} = 16 B */
        mesh.m_vertexBase =
            reinterpret_cast<unsigned char const*>(item->coll_vertices.data());
        mesh.m_vertexStride = sizeof(blam::collision::vertex);
        mesh.m_vertexType   = PHY_FLOAT;

        m_mesh_iface = std::make_unique<btTriangleIndexVertexArray>();
        m_mesh_iface->addIndexedMesh(mesh, PHY_INTEGER);

        auto t2       = steady_clock::now();
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
        mesh.m_numTriangles        = body_create.indices.size() / 3;
        mesh.m_triangleIndexStride = sizeof(IType) * 3;
        mesh.m_indexType           = []() {
            if constexpr(std::is_same_v<IType, u16>)
                return PHY_SHORT;
            if constexpr(std::is_same_v<IType, u32>)
                return PHY_INTEGER;
        }();
        mesh.m_numVertices =
            body_create.vertices.size_bytes() / body_create.vertex_stride;
        mesh.m_vertexStride = body_create.vertex_stride;
        mesh.m_vertexType   = [&body_create]() {
            switch(body_create.vertex_type)
            {
            default:
                return PHY_FLOAT;
            }
        }();

        mesh.m_vertexBase =
            reinterpret_cast<unsigned char const*>(body_create.vertices.data());
        mesh.m_triangleIndexBase =
            reinterpret_cast<unsigned char const*>(body_create.indices.data());

        entity_body.mesh_iface = std::make_unique<btTriangleIndexVertexArray>();
        entity_body.mesh_iface->addIndexedMesh(mesh, mesh.m_indexType);
        entity_body.world_shape = std::make_unique<btBvhTriangleMeshShape>(
            entity_body.mesh_iface.get(), true);
        btRigidBody::btRigidBodyConstructionInfo info{
            body_create.mass,
            nullptr,
            entity_body.world_shape.get(),
        };
        entity_body.world_body = std::make_unique<btRigidBody>(info);
        entity_body.world_body->setFriction(1.f);
        entity_body.world_body->setUserPointer(
            reinterpret_cast<void*>(
                static_cast<uintptr_t>(body_create.entity_id)));
        m_world->addRigidBody(entity_body.world_body.get());
    }

    /* Create body with pre-defined shape
     */
    auto create_body(Physics::BodyCreationShape const& body_create)
    {
        cDebug(
            "Spawning {} shape at {}",
            magic_enum::enum_name(body_create.shape),
            body_create.position);
        entity_body& entity_body = m_bodies[body_create.entity_id];

        /* Same teardown as the mesh path above: the previous body must
         * leave the world before its unique_ptr is overwritten, otherwise
         * the broadphase keeps a dangling pointer and the next
         * stepSimulation() crashes. */
        if(entity_body.world_body)
        {
            detach(entity_body);
            entity_body.world_body.reset();
            entity_body.world_shape.reset();
            entity_body.child_shapes.clear();
            entity_body.mesh_iface.reset();
        }

        switch(body_create.shape)
        {
        case Physics::BodyCreationShape::Capsule: {
            auto capsule = std::make_unique<btCapsuleShape>(
                body_create.scale.x, body_create.scale.z);
            entity_body.world_shape = std::move(capsule);
            break;
        }
        case Physics::BodyCreationShape::Sphere: {
            entity_body.world_shape =
                std::make_unique<btSphereShape>(body_create.scale.x);
            break;
        }
        case Physics::BodyCreationShape::Box: {
            entity_body.world_shape = std::make_unique<btBoxShape>(btVector3(
                body_create.scale.x, body_create.scale.y, body_create.scale.z));
            break;
        }
        case Physics::BodyCreationShape::Hulls: {
            if(!body_create.hulls)
                break;
            auto compound = std::make_unique<btCompoundShape>();
            for(auto const& node : body_create.hulls->nodes)
            {
                auto hull = std::make_unique<btConvexHullShape>();
                for(auto const& pt : node.points)
                {
                    Vecf3 const at = pt + body_create.offset;
                    hull->addPoint(btVector3(at.x, at.y, at.z), false);
                }
                /* Bullet's default 0.04 margin would grow every node */
                hull->setMargin(hull_margin);
                hull->recalcLocalAabb();
                btTransform identity;
                identity.setIdentity();
                compound->addChildShape(identity, hull.get());
                entity_body.child_shapes.push_back(std::move(hull));
            }
            entity_body.world_shape = std::move(compound);
            break;
        }
        case Physics::BodyCreationShape::Mesh: {
            if(!body_create.hulls || body_create.hulls->triangles.empty())
                break;
            auto        mesh = std::make_unique<btTriangleMesh>();
            auto const& tris = body_create.hulls->triangles;
            for(size_t i = 0; i + 2 < tris.size(); i += 3)
            {
                auto pt = [&](Vecf3 const& v) {
                    Vecf3 const at = v + body_create.offset;
                    return btVector3(at.x, at.y, at.z);
                };
                mesh->addTriangle(pt(tris[i]), pt(tris[i + 1]), pt(tris[i + 2]));
            }
            entity_body.world_shape =
                std::make_unique<btBvhTriangleMeshShape>(mesh.get(), true);
            entity_body.mesh_iface = std::move(mesh);
            break;
        }
        }
        /* Primitives are centred on the body; move them off it */
        bool const primitive =
            body_create.shape == Physics::BodyCreationShape::Box ||
            body_create.shape == Physics::BodyCreationShape::Sphere;
        if(primitive && entity_body.world_shape &&
           body_create.offset != Vecf3{})
        {
            auto        compound = std::make_unique<btCompoundShape>();
            btTransform child;
            child.setIdentity();
            child.setOrigin(btVector3(
                body_create.offset.x,
                body_create.offset.y,
                body_create.offset.z));
            compound->addChildShape(child, entity_body.world_shape.get());
            entity_body.child_shapes.push_back(
                std::move(entity_body.world_shape));
            entity_body.world_shape = std::move(compound);
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
        if(body_create.mass > 0.f && body_create.mass_points)
        {
            auto const& moments = body_create.mass_points->moments;
            local_inertia       = btVector3(moments.x, moments.y, moments.z);
        } else if(body_create.mass > 0.f)
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
            /* A vehicle's mass points do its friction; its hulls only
             * stop hard landings */
            entity_body.world_body->setFriction(
                body_create.mass_points ? 0.f : 1.f);
        btTransform transform = m_world_basis;
        if(body_create.rotation)
            transform.setRotation(btQuaternion(
                body_create.rotation->x,
                body_create.rotation->y,
                body_create.rotation->z,
                body_create.rotation->w));
        else if(body_create.shape == Physics::BodyCreationShape::Hulls)
            transform = yaw_basis(body_create.yaw);
        transform.setOrigin(btVector3(
            body_create.position.x,
            body_create.position.y,
            body_create.position.z));
        entity_body.world_body->setWorldTransform(transform);
        entity_body.world_body->setUserPointer(
            reinterpret_cast<void*>(
                static_cast<uintptr_t>(body_create.entity_id)));
        if(body_create.sensor)
            entity_body.world_body->setCollisionFlags(
                entity_body.world_body->getCollisionFlags() |
                btCollisionObject::CF_NO_CONTACT_RESPONSE);
        if(body_create.kinematic)
        {
            entity_body.world_body->setCollisionFlags(
                entity_body.world_body->getCollisionFlags() |
                btCollisionObject::CF_KINEMATIC_OBJECT);
            entity_body.world_body->setActivationState(DISABLE_DEACTIVATION);
        }
        if(body_create.group == Physics::BodyCreationShape::Item)
        {
            /* Small and fast: sweep them so they can't tunnel the ground */
            btVector3 lo, hi;
            entity_body.world_shape->getAabb(transform, lo, hi);
            /* Items are placed by where they rest; started inside the floor
             * they would be pushed out through its back */
            if(f32 const sink = body_create.position.z - lo.z(); sink > 0.f)
            {
                transform.getOrigin().setZ(transform.getOrigin().z() + sink + .01f);
                entity_body.world_body->setWorldTransform(transform);
            }
            btVector3 const extent = hi - lo;
            f32 const       thin =
                std::min({extent.x(), extent.y(), extent.z()}) * .5f;
            entity_body.world_body->setCcdMotionThreshold(thin);
            entity_body.world_body->setCcdSweptSphereRadius(thin * .8f);
            if(body_create.shape == Physics::BodyCreationShape::Sphere)
            {
                entity_body.world_body->setRollingFriction(.02f);
                entity_body.world_body->setSpinningFriction(.02f);
            }
        }
        entity_body.group       = body_create.group;
        entity_body.mass_points = body_create.mass_points;
        add_to_world(*entity_body.world_body, body_create.group);
        if(auto const& points = body_create.mass_points;
           points && !body_create.kinematic && body_create.mass > 0.f)
        {
            btRigidBody& rigid = *entity_body.world_body;
            rigid.setGravity(m_world->getGravity() * points->gravity_scale);
            f32 const air = 1.f - std::pow(
                1.f - std::clamp(points->air_friction, 0.f, 1.f), 30.f);
            rigid.setDamping(air, air);
            entity_body.action = std::make_unique<MassPointAction>(
                rigid, points, body_create.drive);
            m_world->addAction(entity_body.action.get());
        }
    }

    void add_to_world(btRigidBody& body, Physics::BodyCreationShape::group_t group)
    {
        using filter = btBroadphaseProxy::CollisionFilterGroups;
        int constexpr all = filter::AllFilter;
        switch(group)
        {
        case Physics::BodyCreationShape::AnyGroup:
            m_world->addRigidBody(&body);
            break;
        case Physics::BodyCreationShape::Character:
            m_world->addRigidBody(
                &body, filter::CharacterFilter, all ^ filter::DebrisFilter);
            break;
        case Physics::BodyCreationShape::Vehicle:
            m_world->addRigidBody(
                &body, filter::DefaultFilter, all ^ filter::DebrisFilter);
            break;
        case Physics::BodyCreationShape::Grounded:
            /* Mass points carry it; the hulls (a warthog's coll tires are
             * smaller than its mass point ones) only stop hard landings */
            m_world->addRigidBody(
                &body, filter::DefaultFilter, all ^ filter::DebrisFilter);
            break;
        case Physics::BodyCreationShape::Item:
            m_world->addRigidBody(
                &body,
                filter::DebrisFilter,
                filter::StaticFilter | filter::DebrisFilter);
            break;
        }
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
        body.world_body->setUserPointer(
            reinterpret_cast<void*>(
                static_cast<uintptr_t>(body_create.entity_id)));
        m_world->addRigidBody(body.world_body.get());
        wake_dynamic_bodies();
    }

    void remove_body(u64 entity_id)
    {
        auto it = m_bodies.find(entity_id);
        if(it == m_bodies.end())
            return;
        detach(it->second);
        /* Give the debug-marker slot back so repeated join/leave (netcode
         * testing) or body rebuilds don't permanently eat buffer space. */
        if(m_markers)
            m_markers->release_strip(it->second.debug_slot);
        m_bodies.erase(it);
        wake_dynamic_bodies();
    }

    void reset_world()
    {
        cDebug("physics: resetting world for map change");
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
        for(auto& [entity, body] : m_bodies)
        {
            detach(body);
            if(m_markers)
                m_markers->release_strip(body.debug_slot);
        }
        m_bodies.clear();
        m_player_bodies.clear();
        m_object_bodies.clear();
        m_world_item = nullptr;
        /* Bodies are gone; holders no longer carry anything */
        m_grabs.clear();
        m_grab_requests.clear();
        m_reset_holders = true;
        m_built_section = -2;
    }

    void add_impulse(Physics::Impulse const& impulse)
    {
        auto body_it = m_bodies.find(impulse.entity_id);
        if(body_it == m_bodies.end())
            return;
        cDebug(
            "Applying impulse of {} to {}", impulse.impulse, impulse.entity_id);
        entity_body& body = (*body_it).second;
        body.world_body->activate(true);
        body.world_body->applyCentralImpulse(
            btVector3(impulse.impulse.x, impulse.impulse.y, impulse.impulse.z));
    }

    void steer(Physics::Drive const& drive)
    {
        auto it = m_bodies.find(drive.vehicle);
        if(it == m_bodies.end() || !it->second.action)
            return;
        it->second.world_body->activate(true);
        it->second.action->steer(
            drive.throttle,
            btVector3(drive.aim.x, drive.aim.y, drive.aim.z));
    }

    /* The structure BSP surface along a ray, and its collision material */
    void probe_ground(Physics::GroundProbe const& probe)
    {
        if(!m_world_body || !m_world_item || !m_bus)
            return;
        struct closest_t : btCollisionWorld::ClosestRayResultCallback
        {
            using ClosestRayResultCallback::ClosestRayResultCallback;
            int triangle{-1};

            btScalar addSingleResult(
                btCollisionWorld::LocalRayResult& hit, bool normal_in_world)
                override
            {
                triangle = hit.m_localShapeInfo
                               ? hit.m_localShapeInfo->m_triangleIndex
                               : -1;
                return ClosestRayResultCallback::addSingleResult(
                    hit, normal_in_world);
            }
        };
        btVector3 const from(probe.from.x, probe.from.y, probe.from.z);
        btVector3 const to(probe.to.x, probe.to.y, probe.to.z);
        closest_t       ground(from, to);
        ground.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
        m_world->rayTest(from, to, ground);
        if(!ground.hasHit() ||
           ground.m_collisionObject != m_world_body.get() ||
           ground.triangle < 0 ||
           static_cast<size_t>(ground.triangle) >= m_tri_surface.size())
            return;
        u32 const surface = m_tri_surface[ground.triangle];
        if(surface >= m_world_item->coll_surfaces.size())
            return;
        i16 const material = m_world_item->coll_surfaces[surface].material;
        auto      shaders  = m_world_item->mesh->collision_materials.data(
            m_world_item->bsp_magic);
        if(!shaders.has_value() || material < 0 ||
           static_cast<size_t>(material) >= shaders.value().size())
            return;
        btVector3 const      at = ground.m_hitPointWorld;
        Physics::Event       event{Physics::Event::GroundHit};
        Physics::GroundHit   hit{
              .entity_id = probe.entity_id,
              .user      = probe.user,
              .point     = {at.x(), at.y(), at.z()},
              .shader    = &shaders.value()[material].shader,
        };
        m_bus->process(event, &hit);
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
        cDebug(
            "Applying translation of {} to {}",
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
            translation.position.z));
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
        auto const&         surfaces = item.coll_surfaces;
        auto const&         edges    = item.coll_edges;

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
        f32                    max_dev = 0.f;

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
        info.m_startWorldTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
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
        /* Children of a compound world_shape */
        std::vector<std::unique_ptr<btCollisionShape>> child_shapes;
        std::unique_ptr<btCollisionShape>               world_shape;
        std::unique_ptr<btRigidBody>                world_body;
        std::shared_ptr<void>                       keep_alive;
        /* Lazily reserved on first debug draw, released in remove_body() */
        DebugMarkers::strip_slot_t debug_slot{};
        Physics::BodyCreationShape::group_t group{
            Physics::BodyCreationShape::AnyGroup};
        std::shared_ptr<MassPoints const> mass_points;
        std::unique_ptr<MassPointAction>  action;
    };

    /* Out of the world, ahead of the body and its shapes going */
    void detach(entity_body& body)
    {
        if(body.action)
            m_world->removeAction(body.action.get());
        body.action.reset();
        if(body.world_body)
            m_world->removeRigidBody(body.world_body.get());
    }

    std::map<u64, entity_body> m_bodies;

    struct player_body_t
    {
        bool                      kinematic{};
        PlayerInfo::biped_shape_t shape{};
    };

    std::map<u64, player_body_t> m_player_bodies;
    std::map<u64, ObjectPhysics::authority_t> m_object_bodies;
    /* The section the world mesh was built from */
    BSPItem const* m_world_item{nullptr};

    /* Forge grabs, by holder */
    struct grab_t
    {
        u64         object{0};
        btTransform relative; /*!< Object in camera space */
        btVector3   velocity{0, 0, 0}; /*!< Over the last step */
    };
    static constexpr f32           hull_margin  = .005f;
    static constexpr f32           halo_gravity = 9.81f / 3.048f;
    static constexpr f32           grab_range   = 40.f;
    static constexpr f32           step_seconds = 1.f / 60.f;
    std::map<u64, Physics::Grab>   m_grab_requests;
    std::map<u64, grab_t>          m_grabs;
    bool                           m_reset_holders{false};

    DebugMarkers*              m_markers{nullptr};
    DebugMarkers::strip_slot_t m_probe_slot{};

    /* Event bus for outgoing events (Overlap); wired by alloc_physics */
    PhysicsBus* m_bus{};

    /* Index buffer for the world shape + triangle → collision surface
     * mapping (material lookup on hit) */
    std::vector<i32> m_triangles;
    std::vector<u32> m_tri_surface;

    static constexpr f32 probe_radius = 0.2f;

    i16               m_built_section{-2};
    u32               m_frame{0};
    bool              m_marker_spawned{false};
    compo::time_point m_next_process_time{};
};

void alloc_physics(compo::EntityContainer& container)
{
    ProfContext _;
    auto&       phys_bus = container.register_subsystem_inplace<PhysicsBus>();
    auto&       physics =
        container.register_subsystem_inplace<PhysicsSystem<halo_version>>();
    physics.m_bus = &phys_bus;
    phys_bus.addEventFunction<Physics::BodyCreationU16>(
        0, [&physics](Physics::Event&, Physics::BodyCreationU16* body_create) {
            physics.create_body(*body_create);
        });
    phys_bus.addEventFunction<Physics::BodyCreationPrebuilt>(
        0, [&physics](Physics::Event&, Physics::BodyCreationPrebuilt* body) {
            physics.adopt_body(std::move(*body));
        });
    phys_bus.addEventFunction<Physics::BodyCreationShape>(
        0, [&physics](Physics::Event&, Physics::BodyCreationShape* body) {
            physics.create_body(*body);
        });
    phys_bus.addEventFunction<Physics::BodyRemoval>(
        0, [&physics](Physics::Event&, Physics::BodyRemoval* removal) {
            physics.remove_body(removal->entity_id);
        });
    phys_bus.addEventFunction<Physics::Reset>(
        0, [&physics](Physics::Event&, Physics::Reset*) {
            physics.reset_world();
        });
    phys_bus.addEventFunction<Physics::Impulse>(
        0, [&physics](Physics::Event&, Physics::Impulse* impulse) {
            physics.add_impulse(*impulse);
        });
    phys_bus.addEventFunction<Physics::Velocity>(
        0, [&physics](Physics::Event&, Physics::Velocity* velocity) {
            physics.set_linear_velocity(*velocity);
        });
    phys_bus.addEventFunction<Physics::Translate>(
        0, [&physics](Physics::Event&, Physics::Translate* translation) {
            physics.translate(*translation);
        });
    phys_bus.addEventFunction<Physics::GroundProbe>(
        0, [&physics](Physics::Event&, Physics::GroundProbe* probe) {
            physics.probe_ground(*probe);
        });
    phys_bus.addEventFunction<Physics::Drive>(
        0, [&physics](Physics::Event&, Physics::Drive* drive) {
            physics.steer(*drive);
        });
    phys_bus.addEventFunction<Physics::Grab>(
        0, [&physics](Physics::Event&, Physics::Grab* grab) {
            /* A release only matters to a holder that has something */
            if(grab->held || physics.m_grabs.contains(grab->entity_id))
                physics.m_grab_requests[grab->entity_id] = *grab;
        });
    phys_bus.addEventFunction<Physics::ProbeHere>(
        0, [&container, &physics](Physics::Event&, Physics::ProbeHere*) {
            for(auto const& e : container.select<PlayerCamera>())
            {
                auto* info = container.get<PlayerInfo>(e.id());
                if(info->seat_idx != 0)
                    continue;
                auto* camera = container.get<PlayerCamera>(e.id());
                physics.move_probe(camera->camera.position);
                break;
            }
        });
}

#else

#include <coffee/core/debug/formatting.h>

void alloc_physics(compo::EntityContainer& container)
{
    /* The bus stays available so senders (camera movement, teleport, map
     * loading) compile and run unchanged; events just have no consumer. */
    container.register_subsystem_inplace<PhysicsBus>();
    Coffee::cDebug("physics: built without Bullet support, subsystem disabled");
}

#endif
