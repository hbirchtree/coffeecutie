#pragma once

#include <blam/elegy/bitmap.h>
#include <blam/elegy/geometry.h>

#include <array>
#include <limits>
#include <optional>
#include <vector>

namespace h3 {

namespace el = blam::elegy;

/*! Diffuse radiance from linear SH and a dominant light, as Halo 3's
 * ravi_order_2_with_dominant_light; see h3_common.glsl */
inline Vecf3 diffuse_radiance(el::vertex_light_t const& light, Vecf3 const& n)
{
    auto const& sh  = light.sh;
    Vecf3 const lum = {0.212656f, 0.715158f, 0.0721856f};
    Vecf3       dir = {-glm::dot(sh[3], lum), -glm::dot(sh[1], lum), glm::dot(sh[2], lum)};
    dir             = glm::length(dir) > 1e-6f ? glm::normalize(dir) : Vecf3(0.f, 0.f, 1.f);
    Vecf3 out;
    for(int c = 0; c < 3; c++)
    {
        Vecf3 linear = Vecf3(sh[3][c], sh[1][c], -sh[2][c]) + 0.4886025f * dir * light.intensity[c];
        f32   dc     = sh[0][c] - 0.2820948f * light.intensity[c];
        out[c]       = (0.886227f * dc - 1.023328f * glm::dot(n, linear)) / glm::pi<f32>();
    }
    return glm::max(out, Vecf3(0.f)) + std::max(glm::dot(dir, n), 0.f) * light.intensity * 0.281f;
}

/*! As h3_common.glsl's object_light: l0-l3, then the dominant intensity */
inline std::array<Vecf4, 5> packed(el::vertex_light_t const& light)
{
    return {Vecf4(light.sh[0], 0.f), Vecf4(light.sh[1], 0.f), Vecf4(light.sh[2], 0.f), Vecf4(light.sh[3], 0.f),
            Vecf4(light.intensity, 0.f)};
}

/*! The lightmap kept on the CPU: its texels and the triangles mapped onto it */
struct BakedLight
{
    struct Triangle
    {
        std::array<Vecf3, 3> position;
        std::array<Vecf2, 3> uv;
    };

    std::vector<Triangle>        triangles;
    std::vector<std::vector<u8>> layers; /*!< Linearized DXT5, 8 SH + 2 intensity */
    Vecui2                       size{};
    Vecf4                        scale{1.f};
    f32                          intensity_scale{1.f};

    void clear()
    {
        triangles.clear();
        layers.clear();
    }

    /* The baked light on the lightmapped surface under `position`, as
     * objects in Halo 3 take it from the lightmap below them */
    std::optional<el::vertex_light_t> below(Vecf3 const& position, f32* height = nullptr) const
    {
        if(layers.size() != 10 || triangles.empty())
            return std::nullopt;
        f32 const top  = position.z + 0.5f;
        f32       best = -std::numeric_limits<f32>::max();
        Vecf2     uv{};
        for(auto const& t : triangles)
        {
            auto const& p = t.position;
            Vecf2 a{p[0].x, p[0].y}, b{p[1].x, p[1].y}, c{p[2].x, p[2].y};
            f32   d = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
            if(std::abs(d) < 1e-8f)
                continue;
            f32 w0 = ((b.y - c.y) * (position.x - c.x) + (c.x - b.x) * (position.y - c.y)) / d;
            f32 w1 = ((c.y - a.y) * (position.x - c.x) + (a.x - c.x) * (position.y - c.y)) / d;
            f32 w2 = 1.f - w0 - w1;
            if(w0 < 0.f || w1 < 0.f || w2 < 0.f)
                continue;
            f32 z = w0 * p[0].z + w1 * p[1].z + w2 * p[2].z;
            if(z > top || z <= best)
                continue;
            best = z;
            uv   = w0 * t.uv[0] + w1 * t.uv[1] + w2 * t.uv[2];
        }
        if(best == -std::numeric_limits<f32>::max())
            return std::nullopt;
        if(height)
            *height = best;
        return at(std::min(static_cast<u32>(uv.x * size.x), size.x - 1),
                  std::min(static_cast<u32>(uv.y * size.y), size.y - 1));
    }

    /* One lightmap texel's light */
    el::vertex_light_t at(u32 x, u32 y) const
    {
        auto value = [&](u32 pair, f32 s) {
            auto  t0 = el::dxt5_texel(layers[pair * 2], size.x, x, y);
            auto  t1 = el::dxt5_texel(layers[pair * 2 + 1], size.x, x, y);
            Vecf3 dir;
            for(int c = 0; c < 3; c++)
                dir[c] = (t0[c] + t1[c]) / 255.f * 2.f - 2.f;
            return dir * (t0[3] / 255.f) * (t1[3] / 255.f) * s;
        };
        return el::vertex_light_t{
            .sh        = {value(0, scale.x), value(1, scale.y), value(2, scale.z), value(3, scale.w)},
            .intensity = value(4, intensity_scale),
        };
    }

    /*! The sun, as the dominant light of the brightest texels: weighted by
     * their dominant intensity squared, so skylight and lamps barely count */
    std::optional<Vecf3> sun() const
    {
        if(layers.size() != 10 || !size.x || !size.y)
            return std::nullopt;
        Vecf3 const lum{0.212656f, 0.715158f, 0.0721856f};
        Vecf3       sum{0.f};
        for(u32 y = 0; y < size.y; y += 8)
            for(u32 x = 0; x < size.x; x += 8)
            {
                auto const light = at(x, y);
                Vecf3      dir{-glm::dot(light.sh[3], lum), -glm::dot(light.sh[1], lum), glm::dot(light.sh[2], lum)};
                f32 const  bright = glm::dot(light.intensity, lum);
                if(glm::length(dir) > 1e-6f && bright > 0.f)
                    sum += glm::normalize(dir) * bright * bright;
            }
        return glm::length(sum) > 1e-6f ? std::optional(glm::normalize(sum)) : std::nullopt;
    }
};

/*!
 * \brief Stand-in for Halo 3's auto exposure: the scene's mean luminance is
 * taken as the baked light under the camera times a typical albedo, and
 * exposure (in stops) brings it to the camera FX target, inside its range.
 */
struct AutoExposure
{
    bool  enabled{false};
    f32   target{-4.5f}; /*!< auto_exposure_brightness, log2 */
    f32   min{0.f};
    f32   max{0.f};

    static constexpr f32 typical_albedo = 0.35f;

    f32 stops_for(f32 luminance) const
    {
        f32 mean = std::max(luminance * typical_albedo, 1e-6f);
        return std::clamp(target - std::log2(mean), min, max);
    }
};

} // namespace h3
