#pragma once

#include "components.h"
#include "peripherals/semantic/enum/data_types.h"

#include <memory>
#include <optional>

#if defined(FEATURE_ENABLE_BULLET3)
class btTriangleIndexVertexArray;
class btBvhTriangleMeshShape;
#endif

namespace Physics {
struct Event
{
    enum type_t
    {
        None,
        BodyCreationU32,   /*!< Request creating a physics body */
        BodyCreationU16,   /*!< Request creating a physics body */
        BodyCreationShape, /*!< Request physics body based on standard shape */
        BodyCreationPrebuilt, /*!< Adopt a shape built off-thread */
        BodyRemoval,          /*!< Remove a body created by any BodyCreation* */

        Impulse,   /*!< Apply impulse to a body */
        Velocity,  /*!< Apply linear velocity to body, for bipeds */
        Translate, /*!< Apply translation to body, for teleport */

        Overlap, /*!< Collision event between two bodies */

        ProbeHere, /*!< Put debug probe at camera position, for testing */

        Grab,    /*!< Forge-style carrying of an object in front of a camera */
        Drive,   /*!< A driver's throttle and aim for a vehicle */
        GroundProbe, /*!< What is underfoot: answered with GroundHit */
        GroundHit,
        Grabbed, /*!< A holder picked an object up */
        Dropped, /*!< A holder let go, or its object went away */
        Impact,  /*!< A body struck the world or another body, audibly */

        Reset, /*!< Tear down all bodies + the world mesh ahead of a map
                * change. Mesh-based bodies reference their source
                * geometry zero-copy (the world BVH reads straight out of
                * the mapped map file), so everything must be out of the
                * simulation before the old map's memory goes away. */
    } type{};
};

template<typename IType, Event::type_t Type>
struct BodyCreation
{
    static constexpr auto event_type = Type;
    u64                   entity_id{0}; /*!< Target entity,
                                         * reference is put into PhysicsData */
    gsl::span<IType>      indices{};
    gsl::span<const char> vertices{};
    u32                   vertex_stride{12};
    semantic::type_t      vertex_type{semantic::type_t::f32};

    Vecf3 position{};
    f32   mass{0.f};

    bool static_body{true};
};

using BodyCreationU32 = BodyCreation<u32, Event::BodyCreationU32>;
using BodyCreationU16 = BodyCreation<u16, Event::BodyCreationU16>;

#if defined(FEATURE_ENABLE_BULLET3)
/* A collision shape constructed on a worker thread (BVH build is the
 * expensive part) and handed over ready-made — the receiving side only
 * wraps it in a rigid body and inserts it into the world. keep_alive
 * owns whatever memory mesh_iface points into (zero-copy meshes). */
struct BodyCreationPrebuilt
{
    static constexpr auto event_type = Event::BodyCreationPrebuilt;
    u64                   entity_id{0};
    std::unique_ptr<btTriangleIndexVertexArray> mesh_iface;
    std::unique_ptr<btBvhTriangleMeshShape>     shape;
    std::shared_ptr<void>                       keep_alive;
    bool                                        static_body{true};
};
#endif

struct BodyCreationShape
{
    static constexpr auto event_type = Event::BodyCreationShape;
    u64                   entity_id{0};
    Vecf3                 scale{1, 1, 1};
    Vecf3                 position{};
    f32 mass{0.f}; /*!< 0 = static/immovable (Bullet convention); >0 makes
                    * the body dynamic and simulated. */

    enum shape_t
    {
        Capsule,
        Sphere,
        Box,
        Hulls, /*!< One convex hull per node, from `hulls` */
        Mesh,  /*!< Exact triangles from `hulls`, static only */
    } shape{Capsule};

    /*! Who touches whom: items only meet the world and each other */
    enum group_t
    {
        AnyGroup, /*!< Bullet's default for the body type */
        Character,
        Vehicle,
        Grounded, /*!< A vehicle meeting the world through `mass_points` */
        Item,
    } group{AnyGroup};

    /*!< Overlap sensor: detected but never collided with
     * (CF_NO_CONTACT_RESPONSE); pairs touching a sensor emit
     * Physics::Overlap events. For trigger volumes (map links). */
    bool sensor{false};

    bool kinematic{false}; /*!< Moved by its owner, not simulated */

    struct
    {
        bool rotation{false};
    } lock{};

    /*! Geometry is moved by `offset` (body space, +Z up). Hull bodies
     * face +X turned by `yaw` about +Z unless `rotation` is given. */
    std::shared_ptr<CollisionGeometry const> hulls;
    Vecf3                                    offset{};
    f32                                      yaw{0.f};
    std::optional<Quatf>                     rotation;
    /*! Body mass, inertia and ground contact from a phys tag; the body
     * origin is its centre of mass */
    std::shared_ptr<MassPoints const> mass_points;
    std::optional<VehicleDrive>       drive;
    /*! Hull children follow the entity's AnimationPlayback::pose */
    bool posed{false};
};

struct BodyRemoval
{
    static constexpr auto event_type = Event::BodyRemoval;
    u64                   entity_id{0};
};

struct Impulse
{
    static constexpr auto event_type = Event::Impulse;
    u64                   entity_id{0};
    Vecf3                 impulse{}; /*!< Force + direction */
};

struct Velocity
{
    static constexpr auto event_type = Event::Velocity;
    u64                   entity_id{0};
    Vecf3                 velocity{}; /*!< Force + direction */
    bool preserve_z{false};           /*!< Keep the body's current Z velocity so
                                       * gravity/falling integrates normally;
                                       * velocity.z is ignored */
    f32 jump{0.f}; /*!< If non-zero, set Z velocity to this value — but only
                    * when the body is vertically at rest (grounded-ish).
                    * Applied on top of preserve_z. */
};

struct Translate
{
    static constexpr auto event_type = Event::Translate;
    u64                   entity_id{0};
    Vecf3                 position{}; /*!< Force + direction */
    bool                  preserve_momentum{false};
};

struct Overlap
{
    static constexpr auto event_type = Event::Overlap;
    u64                   entity_id_1{}, entity_id_2{};
};

/*! Sent every frame per local camera. While `held`, the first moving
 * object along the view is carried kinematically, keeping where it sat
 * relative to the camera; let go, it is simulated again */
struct Grab
{
    static constexpr auto event_type = Event::Grab;
    u64                   entity_id{0}; /*!< Holder */
    Vecf3                 origin{};
    Quatf to_world{}; /*!< Camera to world; the view is its -Z */
    bool                  held{false};
};

/*! Sent every frame while a driver sits in `vehicle`. The vehicle heads
 * where `aim` points (the driver's view), at `throttle` of its top speed
 * forward, or backward when negative. */
struct Drive
{
    static constexpr auto event_type = Event::Drive;
    u64                   vehicle{0};
    f32                   throttle{0.f};
    Vecf3                 aim{1, 0, 0};
};

/*! Asks what the world is made of along from -> to; answered at once with
 * a GroundHit when it hits the structure BSP */
struct GroundProbe
{
    static constexpr auto event_type = Event::GroundProbe;
    u64                   entity_id{0};
    Vecf3                 from{};
    Vecf3                 to{};
    u32                   user{0}; /*!< Handed back in the hit */
};

struct GroundHit
{
    static constexpr auto event_type = Event::GroundHit;
    u64                   entity_id{0};
    u32                   user{0};
    Vecf3                 point{};
    /*! The surface's collision material, a shader with its physics material */
    blam::tagref_t const* shader{nullptr};
};

struct Grabbed
{
    static constexpr auto event_type = Event::Grabbed;
    u64                   holder{0};
    u64                   object{0};
};

struct Dropped
{
    static constexpr auto event_type = Event::Dropped;
    u64                   holder{0};
    u64                   object{0};
    Vecf3                 velocity{}; /*!< It was let go with */
};

/*! A dynamic body hit something hard enough to be heard */
struct Impact
{
    static constexpr auto event_type = Event::Impact;
    u64                   entity_id{0};
    u64                   other{0}; /*!< Body it hit, 0 for the world */
    Vecf3                 point{};
    f32                   speed{0.f}; /*!< Closing speed taken out, wu/s */
    /*! The world surface's collision material, when it hit the world */
    blam::tagref_t const* shader{nullptr};
};

struct ProbeHere
{
    static constexpr auto event_type = Event::ProbeHere;
};

struct Reset
{
    static constexpr auto event_type = Event::Reset;
};

} // namespace Physics

using PhysicsBus = comp_app::BasicEventBus<Physics::Event>;

void alloc_physics(compo::EntityContainer& container);
