#pragma once

#include "components.h"
#include "peripherals/semantic/enum/data_types.h"

#include <memory>

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
        BodyCreationU32, /*!< Request creating a physics body */
        BodyCreationU16, /*!< Request creating a physics body */
        BodyCreationShape, /*!< Request physics body based on standard shape */
        BodyCreationPrebuilt, /*!< Adopt a shape built off-thread */
        BodyRemoval, /*!< Remove a body created by any BodyCreation* */

        Impulse, /*!< Apply impulse to a body */
        Velocity, /*!< Apply linear velocity to body, for bipeds */
        Translate, /*!< Apply translation to body, for teleport */
        
        Overlap, /*!< Collision event between two bodies */
        
        ProbeHere, /*!< Put debug probe at camera position, for testing */
    } type{};
};

template<typename IType, Event::type_t Type>
struct BodyCreation
{
    static constexpr auto event_type = Type;
    u64 entity_id{0}; /*!< Target entity, 
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
    u64 entity_id{0};
    Vecf3 scale{1, 1, 1};
    Vecf3 position{};
    f32   mass{0.f}; /*!< 0 = static/immovable (Bullet convention); >0 makes
                       * the body dynamic and simulated. */
    enum shape_t
    {
        Capsule,
        Sphere,
        Box,
    } shape{Capsule};
    /*!< Overlap sensor: detected but never collided with
     * (CF_NO_CONTACT_RESPONSE); pairs touching a sensor emit
     * Physics::Overlap events. For trigger volumes (map links). */
    bool sensor{false};
    struct
    {
        bool rotation{false};
    } lock;
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
    bool preserve_z{false}; /*!< Keep the body's current Z velocity so
                             * gravity/falling integrates normally;
                             * velocity.z is ignored */
    f32  jump{0.f}; /*!< If non-zero, set Z velocity to this value — but only
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
    u64 entity_id_1{}, entity_id_2{};
};

struct ProbeHere
{
    static constexpr auto event_type = Event::ProbeHere;
};

}

using PhysicsBus = comp_app::BasicEventBus<Physics::Event>;

void alloc_physics(compo::EntityContainer& container);
