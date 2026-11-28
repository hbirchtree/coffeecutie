#pragma once

#include "loading/blam_files.h"

#include <algorithm>
#include <map>
#include <vector>

namespace hills {

constexpr libc_types::f32 below  = .5f;
constexpr libc_types::f32 above  = 2.5f;
constexpr libc_types::f32 radius = 2.f;

/* A king of the hill hill: the ground its flags outline (their convex hull)
 * and some height above it */
struct hill_t
{
    using Vecf2 = typing::vector_types::Vecf2;
    using Vecf3 = typing::vector_types::Vecf3;

    std::vector<Vecf3> hull; /*!< Counter-clockwise, at each flag's height */
    Vecf3              centre{};
    libc_types::f32    low{0.f}, high{0.f};

    bool holds(Vecf3 const& at) const
    {
        if(at.z < low - below || at.z > high + above)
            return false;
        if(hull.size() < 3)
            return glm::distance(Vecf2(at), Vecf2(centre)) <= radius;
        for(size_t i = 0; i < hull.size(); i++)
        {
            Vecf2 const a = hull[i], b = hull[(i + 1) % hull.size()];
            if((b.x - a.x) * (at.y - a.y) - (b.y - a.y) * (at.x - a.x) < 0.f)
                return false;
        }
        return true;
    }
};

/* Monotone chain over x and y */
inline std::vector<typing::vector_types::Vecf3> convex_hull(std::vector<typing::vector_types::Vecf3> points)
{
    using Vecf3 = typing::vector_types::Vecf3;
    std::sort(points.begin(), points.end(), [](auto const& a, auto const& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });
    if(points.size() < 3)
        return points;
    auto const cross = [](Vecf3 o, Vecf3 a, Vecf3 b) { return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x); };
    std::vector<Vecf3> hull(points.size() * 2);
    size_t             k = 0;
    for(size_t i = 0; i < points.size(); hull[k++] = points[i++])
        while(k >= 2 && cross(hull[k - 2], hull[k - 1], points[i]) <= 0.f)
            k--;
    for(size_t i = points.size() - 1, t = k + 1; i > 0; hull[k++] = points[--i])
        while(k >= t && cross(hull[k - 2], hull[k - 1], points[i - 1]) <= 0.f)
            k--;
    hull.resize(k - 1);
    return hull;
}

/* Each hill is the hill_flag netgame flags of one index */
template<typename V>
std::vector<hill_t> of(BlamFiles<V> const& files)
{
    std::vector<hill_t> out;
    auto const*         scenario = files.container.scenario().value_or(nullptr);
    if(!scenario)
        return out;
    auto flags = scenario->netgame.flags.data(files.container.magic);
    if(!flags.has_value())
        return out;
    std::map<libc_types::u16, std::vector<typing::vector_types::Vecf3>> corners;
    for(auto const& flag : flags.value())
        if(flag.type == blam::scn::multiplayer_flag::flag_type_t::hill_flag)
            corners[flag.team_index].push_back(flag.pos);
    for(auto const& [_, points] : corners)
    {
        hill_t hill{.low = points.front().z, .high = points.front().z};
        for(auto const& at : points)
        {
            hill.centre += at / static_cast<libc_types::f32>(points.size());
            hill.low  = std::min(hill.low, at.z);
            hill.high = std::max(hill.high, at.z);
        }
        hill.hull = convex_hull(points);
        out.push_back(std::move(hill));
    }
    return out;
}

} // namespace hills
