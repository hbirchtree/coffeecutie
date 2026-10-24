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

/*! A bounding sphere (centre, radius) where `transform` places it */
inline Vecf4 place_sphere(Matf4 const& transform, Vecf4 const& sphere)
{
    f32 const scale = std::max(
        {glm::length(Vecf3(transform[0])), glm::length(Vecf3(transform[1])), glm::length(Vecf3(transform[2]))});
    return Vecf4(Vecf3(transform * Vecf4(Vecf3(sphere), 1.f)), sphere.w * scale);
}

} // namespace world
