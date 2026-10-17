#include "camera_control.h"

#include "components.h"
#include "data.h"
#include "player_controller.h"
#include "proxy.h"

#include <chrono>
#include <glm/common.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <peripherals/stl/time_types.h>

using namespace std::chrono_literals;
using stl_types::chrono::to_f32;

namespace {

using lerp_t = CameraLerp::lerp_t;

/* Cubic Hermite from p1 with velocity v1 (over the whole lerp) to p2 at
 * rest: eases in and out, and keeps a cut-in lerp's motion going */
Vecf3 hermite(lerp_t const& lerp, f32 s)
{
    f32 const s2 = s * s, s3 = s2 * s;
    return lerp.p1 * (2.f * s3 - 3.f * s2 + 1.f) + lerp.v1 * (s3 - 2.f * s2 + s) +
           lerp.p2 * (-2.f * s3 + 3.f * s2);
}

Vecf3 hermite_velocity(lerp_t const& lerp, f32 s)
{
    f32 const s2 = s * s;
    return lerp.p1 * (6.f * s2 - 6.f * s) + lerp.v1 * (3.f * s2 - 4.f * s + 1.f) +
           lerp.p2 * (-6.f * s2 + 6.f * s);
}

/* Turns ease in from rest, or only out when cutting into another turn */
f32 turn(lerp_t const& lerp, f32 s)
{
    bool const moving = glm::length(lerp.v1) > 0.f;
    return moving ? s * (2.f - s) : s * s * (3.f - 2.f * s);
}

/* A world rotation as the view rotation sees it: the view applies after the
 * permutation build_camera_matrix folds in */
Quatf in_view_basis(Matf4 const& space)
{
    static Matf3 const basis{
        {0, 0, 1},
        {1, 0, 0},
        {0, 1, 0},
    };
    return glm::normalize(Quatf(basis * Matf3(space) * glm::transpose(basis)));
}

f32 progress(lerp_t const& lerp)
{
    return glm::clamp(
        1.f - to_f32(lerp.remaining_time) / std::max(to_f32(lerp.total_time), 1e-6f),
        0.f,
        1.f);
}

} // namespace

struct CameraControl
    : compo::RestrictedSubsystem<CameraControl, CameraControlManifest>
{
    using type  = CameraControl;
    using Proxy = compo::proxy_of<CameraControlManifest>;

    CameraControl()
    {
        // Run just before MeshRenderer to keep it up to date
        this->priority = 3070;
    }

    /* The cinematic view in its own space, and the transform out of it */
    static Matf4 space_of(Proxy& p, u64 relative_to)
    {
        if(auto const* model = relative_to ? p.get<Model>(relative_to) : nullptr)
            return glm::translate(Matf4(1), model->position) * glm::mat4_cast(model->rotation);
        return Matf4(1);
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        for(auto const& control : std::exchange(incoming_controls, {}))
            for(auto camera : p.select<PlayerInfo, PlayerCamera, CameraLerp>())
            {
                auto [info, cam, lerp] = camera.components();
                if(info.seat_idx != control.seat_idx)
                    continue;
                std::erase_if(lerp.lerps, [](lerp_t const& l) { return l.cinematic; });
                if(!control.control)
                    cam.cinematic.reset();
                else if(!cam.cinematic)
                    cam.cinematic = PlayerCamera::cinematic_t{
                        .position = cam.camera.position,
                        .rotation = cam.camera.rotation,
                        .fov      = cam.camera.fieldOfView,
                    };
            }

        for(auto const& new_lerp : incoming_lerps)
            for(auto camera : p.select<PlayerInfo, PlayerCamera, CameraLerp>())
            {
                auto [info, cam, lerp] = camera.components();
                if(info.seat_idx != new_lerp.seat_idx)
                    continue;
                if(new_lerp.cinematic)
                {
                    begin_cinematic(p, cam, lerp, new_lerp);
                    continue;
                }
                if(new_lerp.duration == 0ms)
                {
                    cam.camera.position = new_lerp.position;
                    if(new_lerp.rotation)
                        cam.camera.rotation = *new_lerp.rotation;
                    continue;
                }
                start(lerp, new_lerp, cam.camera.position, cam.camera.rotation, 0.f);
            }
        incoming_lerps.clear();

        /* First tick after registration has no previous timestamp to
         * subtract, and the epoch is far in the past -- without this every
         * lerp would be retired the moment it arrived. */
        if(last_time == time_point{})
            last_time = t;
        auto reduction = std::chrono::duration_cast<std::chrono::milliseconds>(
            t - last_time);
        for(auto camera_ : p.select<PlayerCamera, CameraLerp>())
        {
            auto [camera, lerps] = camera_.components();
            for(auto& lerp : lerps.lerps)
            {
                lerp.remaining_time -= reduction;
                /* Advance first, then clamp: the last tick takes remaining
                 * below zero, and this is where the lerp has to land exactly
                 * on its target before being retired below. */
                f32 const   s        = progress(lerp);
                Vecf3 const position = hermite(lerp, s);
                Quatf const rotation = glm::slerp(lerp.r1, lerp.r2, turn(lerp, s));
                if(lerp.cinematic && camera.cinematic)
                {
                    camera.cinematic->position    = position;
                    camera.cinematic->rotation    = rotation;
                    camera.cinematic->fov         = glm::mix(lerp.fov1, lerp.fov2, s);
                    camera.cinematic->relative_to = lerp.relative_to;
                } else if(!lerp.cinematic)
                {
                    camera.camera.position = position;
                    camera.camera.rotation = rotation;
                }
            }
            std::erase_if(lerps.lerps, [](auto const& lerp) {
                return lerp.remaining_time <= 0ms;
            });
        }
        last_time = t;

        /* The cutscene camera is what gets drawn */
        for(auto camera : p.select<PlayerCamera>())
        {
            auto& cam = camera.get<PlayerCamera>();
            if(!cam.cinematic)
                continue;
            Matf4 const space = space_of(p, cam.cinematic->relative_to);
            Vecf3 const eye   = Vecf3(space * Vecf4(cam.cinematic->position, 1.f));
            Quatf const look  = cam.cinematic->relative_to
                                    ? cam.cinematic->rotation * glm::inverse(in_view_basis(space))
                                    : cam.cinematic->rotation;
            auto view               = cam;
            view.camera.rotation    = look;
            view.camera.fieldOfView = cam.cinematic->fov;
            build_camera_matrix(view, eye);
            cam.matrix    = view.matrix;
            cam.rotation  = view.rotation;
            cam.drawn_eye = eye;
        }
    }

    /* A cutscene camera move: from where the view is now, carrying on any
     * move it cuts into */
    void begin_cinematic(
        Proxy& p, PlayerCamera& cam, CameraLerp& lerps, PlayerCameraLerpEvent const& to)
    {
        if(!cam.cinematic)
            cam.cinematic = PlayerCamera::cinematic_t{
                .position = cam.camera.position,
                .rotation = cam.camera.rotation,
                .fov      = cam.camera.fieldOfView,
            };
        auto& view = *cam.cinematic;
        /* Into the target's space, so a relative move ends on it */
        Matf4 const from_space = space_of(p, view.relative_to);
        Matf4 const to_space   = glm::inverse(space_of(p, to.relative_to));
        Matf4 const change     = to_space * from_space;
        Vecf3 velocity{};
        for(auto const& running : lerps.lerps)
            if(running.cinematic && running.remaining_time > 0ms)
                velocity = Matf3(change) * hermite_velocity(running, progress(running)) /
                           std::max(to_f32(running.total_time), 1e-6f);
        std::erase_if(lerps.lerps, [](lerp_t const& l) { return l.cinematic; });

        view.position    = Vecf3(change * Vecf4(view.position, 1.f));
        view.rotation    = view.rotation * glm::inverse(in_view_basis(change));
        view.relative_to = to.relative_to;
        if(to.duration == 0ms)
        {
            view.position = to.position;
            view.rotation = to.rotation.value_or(view.rotation);
            view.fov      = to.fov.value_or(view.fov);
            return;
        }
        start(lerps, to, view.position, view.rotation, view.fov, velocity * to_f32(to.duration));
        lerps.lerps.back().cinematic   = true;
        lerps.lerps.back().relative_to = to.relative_to;
    }

    static void start(
        CameraLerp&                  lerps,
        PlayerCameraLerpEvent const& to,
        Vecf3 const&                 position,
        Quatf const&                 rotation,
        f32                          fov,
        Vecf3 const&                 velocity = {})
    {
        lerps.lerps.push_back(lerp_t{
            .p1             = position,
            .p2             = to.position,
            .r1             = rotation,
            .r2             = to.rotation.value_or(rotation),
            .remaining_time = to.duration,
            .total_time     = to.duration,
            .v1             = velocity,
            .fov1           = fov,
            .fov2           = to.fov.value_or(fov),
        });
    }

    time_point                         last_time;
    std::vector<PlayerCameraLerpEvent> incoming_lerps;
    std::vector<CinematicCameraEvent>  incoming_controls;
};

void alloc_camera_control(compo::EntityContainer& e)
{
    auto& camera   = e.register_subsystem_inplace<CameraControl>();
    auto& game_bus = e.subsystem_cast<GameEventBus>();
    game_bus.addEventFunction<PlayerCameraLerpEvent>(
        1024, [&camera](GameEvent&, PlayerCameraLerpEvent* lerp) {
            cDebug(
                "Lerp'ing camera #{} to {} in {}ms",
                lerp->seat_idx,
                lerp->position,
                lerp->duration.count());
            camera.incoming_lerps.push_back(*lerp);
        });
    game_bus.addEventFunction<CinematicCameraEvent>(
        1024, [&camera](GameEvent&, CinematicCameraEvent* control) {
            camera.incoming_controls.push_back(*control);
        });
    game_bus.addEventData({
        1024, [&camera](GameEvent& e, void*) {
            if(e.type != GameEvent::MapAllLoaded)
                return;
            camera.incoming_lerps.clear();
            camera.incoming_controls.clear();
        },
    });
}
