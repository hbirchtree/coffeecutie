#pragma once

/* View angles from raw look input. The client and the server run this same
 * function over the same input, so the server's idea of where a player is
 * looking comes from what the player did, never from an angle the client
 * reports. */

#include "events.h"

#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace poc {

struct LookConfig
{
    float mouse_rad_per_count{0.0005f};
    float stick_rad_per_second{3.5f}; /* at full deflection */
    float pitch_limit{1.45f};
};

inline float wrap_angle(float a)
{
    return std::remainder(a, glm::two_pi<float>());
}

/* Mouse right / stick right turns right (yaw down), mouse down looks down */
inline View apply_look(View v, LookInput const& in, LookConfig const& c)
{
    float const stick = c.stick_rad_per_second * tick_seconds / 127.f;
    v.yaw   = wrap_angle(v.yaw - in.mouse_dx * c.mouse_rad_per_count -
                       in.stick_x * stick);
    v.pitch = std::clamp(
        v.pitch - in.mouse_dy * c.mouse_rad_per_count - in.stick_y * stick,
        -c.pitch_limit,
        c.pitch_limit);
    return v;
}

inline vec3 forward(View const& v)
{
    return {
        std::cos(v.pitch) * std::cos(v.yaw),
        std::cos(v.pitch) * std::sin(v.yaw),
        std::sin(v.pitch)};
}

inline View view_of(vec3 const& dir)
{
    return {
        std::atan2(dir.y, dir.x),
        std::atan2(dir.z, std::sqrt(dir.x * dir.x + dir.y * dir.y))};
}

/* Angle between two view directions, radians */
inline float angle_between(vec3 const& a, vec3 const& b)
{
    return std::acos(std::clamp(
        glm::dot(glm::normalize(a), glm::normalize(b)), -1.f, 1.f));
}

inline float angle_between(View const& a, View const& b)
{
    return angle_between(forward(a), forward(b));
}

/* Mouse counts that turn `from` as close to `to` as the quantization allows */
inline LookInput mouse_towards(View const& from, View const& to, LookConfig const& c)
{
    auto counts = [&](float rad) {
        return static_cast<std::int16_t>(std::clamp(
            std::lround(rad / c.mouse_rad_per_count), -32767l, 32767l));
    };
    return {
        .mouse_dx = counts(-wrap_angle(to.yaw - from.yaw)),
        .mouse_dy = counts(-(to.pitch - from.pitch)),
    };
}

} // namespace poc
