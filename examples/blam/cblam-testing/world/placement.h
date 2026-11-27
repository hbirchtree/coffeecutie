#pragma once

#include "../components.h"

#include <algorithm>
#include <cmath>

namespace world {

/*! Two neighbouring frames of a looping animation and the blend between */
struct frame_blend_t
{
    u32 f0, f1;
    f32 t;
};

/*! The frames around `at` frames into a loop of `frames` */
inline frame_blend_t loop_frame(f32 at, u32 frames)
{
    at           = std::fmod(at, static_cast<f32>(frames));
    u32 const f0 = static_cast<u32>(at) % frames;
    return {f0, (f0 + 1) % frames, at - std::floor(at)};
}

/*! The frames around `at` frames into a clip played once, held on its last */
inline frame_blend_t once_frame(f32 at, u32 frames)
{
    at           = std::clamp(at, 0.f, static_cast<f32>(frames - 1));
    u32 const f0 = static_cast<u32>(at);
    return {f0, std::min(f0 + 1, frames - 1), at - std::floor(at)};
}

/*! What a view model plays once over its idle, from what the local
 * player's weapon does: each shot, and each reload as it begins */
struct view_actions_t
{
    enum kind_t : u8
    {
        none,
        fire,
        reload_full,
        reload_empty,
    };

    u16  shots{0};
    bool reloading{false};

    /*! The action begun since the last look, if any */
    kind_t observe(BipedEquipment const& equip)
    {
        kind_t out = none;
        if(equip.reloading && !reloading)
            out = equip.weapons[0].rounds_loaded == 0 ? reload_empty : reload_full;
        else if(equip.shots != shots)
            out = fire;
        shots     = equip.shots;
        reloading = equip.reloading;
        return out;
    }
};

/*! A bounding sphere (centre, radius) where `transform` places it */
inline Vecf4 place_sphere(Matf4 const& transform, Vecf4 const& sphere)
{
    f32 const scale = std::max(
        {glm::length(Vecf3(transform[0])), glm::length(Vecf3(transform[1])), glm::length(Vecf3(transform[2]))});
    return Vecf4(Vecf3(transform * Vecf4(Vecf3(sphere), 1.f)), sphere.w * scale);
}

} // namespace world
