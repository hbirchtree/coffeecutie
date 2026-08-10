#include "camera_control.h"

#include "components.h"
#include "data.h"
#include "proxy.h"

#include <chrono>
#include <glm/common.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/gtx/spline.hpp>
#include <peripherals/stl/time_types.h>

using namespace std::chrono_literals;
using stl_types::chrono::to_f32;

struct CameraControl 
    : compo::RestrictedSubsystem<CameraControl, CameraControlManifest>
{
    using type = CameraControl;
    using Proxy = compo::proxy_of<CameraControlManifest>;

    CameraControl()
    {
        // Run just before MeshRenderer to keep it up to date
        this->priority = 3070;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        for(auto const& new_lerp : incoming_lerps)
            for(auto camera : p.select<PlayerInfo, PlayerCamera, CameraLerp>())
            {
                auto [info, cam, lerp] = camera.components();
                if(info.seat_idx != new_lerp.seat_idx)
                    continue;
                if(new_lerp.duration == 0ms)
                {
                    cam.camera.position = new_lerp.position;
                    if(new_lerp.rotation)
                        cam.camera.rotation = *new_lerp.rotation;
                    continue;
                }
                auto from_pos = cam.camera.position;
                auto to_pos   = new_lerp.position;
                auto from_rot = cam.camera.rotation;
                auto to_rot   = new_lerp.rotation;
                lerp.lerps.push_back(CameraLerp::lerp_t{
                    .p1             = from_pos,
                    .p2             = to_pos,
                    .r1             = from_rot,
                    .r2             = to_rot.value_or(from_rot),
                    .remaining_time = new_lerp.duration,
                    .total_time     = new_lerp.duration,
                });
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
                f32 alpha = 1.f - to_f32(lerp.remaining_time) /
                                      to_f32(lerp.total_time);
                alpha = glm::clamp(alpha, 0.f, 1.f);
                /* Assigned, not accumulated: p1 is the start point, so the
                 * interpolation is absolute. Adding the offset each tick
                 * compounds it and the position runs away. */
                camera.camera.position = lerp.p1 + (lerp.p2 - lerp.p1) * alpha;
                camera.camera.rotation = glm::slerp(lerp.r1, lerp.r2, alpha);
            }
            std::erase_if(lerps.lerps, [](auto const& lerp) {
                return lerp.remaining_time <= 0ms;
            });
        }
        last_time = t;
    }

    time_point                         last_time;
    std::vector<PlayerCameraLerpEvent> incoming_lerps;
};

void alloc_camera_control(compo::EntityContainer& e)
{
    auto& camera = e.register_subsystem_inplace<CameraControl>();
    auto& game_bus = e.subsystem_cast<GameEventBus>();
    game_bus.addEventFunction<PlayerCameraLerpEvent>(
        1024, [&camera](GameEvent&, PlayerCameraLerpEvent* lerp) {
            cDebug("Lerp'ing camera #{} to {} in {}ms",
                lerp->seat_idx,
                lerp->position,
                lerp->duration.count());
            camera.incoming_lerps.push_back(*lerp);
        });
}
