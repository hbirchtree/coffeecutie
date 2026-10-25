#pragma once

/* Model reflections on GX, which has no cube maps: a shader's reflection cube
 * is decoded once into small faces, and each frame it is baked into a sphere
 * map for the camera's orientation. Models then read the sphere map through a
 * texture coordinate generated from their eye-space normals. */

#include <blam/common/endian.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <vector>

namespace gx_reflection {

using blam::from_le;
using libc_types::f32;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

/* Small faces in API order (+X, -X, +Y, -Y, +Z, -Z), RGB */
struct cube_t
{
    u32                            size{0};
    std::array<std::vector<u8>, 6> faces;
};

enum class source_format
{
    bc1,
    bc2_bc3, // colour block after 8 bytes of alpha
    rgb565,
    bgra8, // A8R8G8B8 / X8R8G8B8, stored B, G, R, A
};

namespace detail {

inline void rgb565(u16 c, u8* out)
{
    out[0] = static_cast<u8>(((c >> 11) & 0x1F) * 255 / 31);
    out[1] = static_cast<u8>(((c >> 5) & 0x3F) * 255 / 63);
    out[2] = static_cast<u8>((c & 0x1F) * 255 / 31);
}

/* One texel of a DXT colour block */
inline void dxt_texel(u8 const* block, u32 x, u32 y, u8* out)
{
    u16 c0, c1;
    std::memcpy(&c0, block, 2);
    std::memcpy(&c1, block + 2, 2);
    c0 = from_le(c0);
    c1 = from_le(c1);
    u8 p[4][3];
    rgb565(c0, p[0]);
    rgb565(c1, p[1]);
    for(u32 i = 0; i < 3; i++)
    {
        p[2][i] = static_cast<u8>((2 * p[0][i] + p[1][i]) / 3);
        p[3][i] = static_cast<u8>((p[0][i] + 2 * p[1][i]) / 3);
    }
    u32 const idx = (block[4 + y] >> (2 * x)) & 3;
    std::memcpy(out, p[idx], 3);
}

} // namespace detail

/* Decode level 0 of one face, point-sampled down to `size` */
inline std::vector<u8> decode_face(
    u8 const* src, source_format fmt, u32 w, u32 size)
{
    std::vector<u8> out(size * size * 3);
    u32 const       bw = (w + 3) / 4;
    for(u32 y = 0; y < size; y++)
        for(u32 x = 0; x < size; x++)
        {
            u32 const sx = x * w / size, sy = y * w / size;
            u8*       o = &out[(y * size + x) * 3];
            switch(fmt)
            {
            case source_format::bc1:
                detail::dxt_texel(
                    src + ((sy / 4) * bw + sx / 4) * 8, sx % 4, sy % 4, o);
                break;
            case source_format::bc2_bc3:
                detail::dxt_texel(
                    src + ((sy / 4) * bw + sx / 4) * 16 + 8, sx % 4, sy % 4, o);
                break;
            case source_format::rgb565: {
                u16 c;
                std::memcpy(&c, src + (sy * w + sx) * 2, 2);
                detail::rgb565(from_le(c), o);
                break;
            }
            case source_format::bgra8: {
                u8 const* p = src + (sy * w + sx) * 4;
                o[0]        = p[2];
                o[1]        = p[1];
                o[2]        = p[0];
                break;
            }
            }
        }
    return out;
}

/* Halo's cubes are Y-up and left-handed: a world direction d samples the
 * cube at (-d.y, d.z, d.x). Standard cube face selection from there. */
inline void sample(cube_t const& cube, glm::vec3 const& world, u8* out)
{
    glm::vec3 const d{-world.y, world.z, world.x};
    glm::vec3 const a = glm::abs(d);
    u32             face;
    f32             sc, tc, ma;
    if(a.x >= a.y && a.x >= a.z)
    {
        face = d.x > 0 ? 0 : 1;
        sc   = d.x > 0 ? -d.z : d.z;
        tc   = -d.y;
        ma   = a.x;
    } else if(a.y >= a.z)
    {
        face = d.y > 0 ? 2 : 3;
        sc   = d.x;
        tc   = d.y > 0 ? d.z : -d.z;
        ma   = a.y;
    } else
    {
        face = d.z > 0 ? 4 : 5;
        sc   = d.z > 0 ? d.x : -d.x;
        tc   = -d.y;
        ma   = a.z;
    }
    u32 const n = cube.size;
    u32 const x = std::min(n - 1, static_cast<u32>((sc / ma + 1.f) * 0.5f * n));
    u32 const y = std::min(n - 1, static_cast<u32>((tc / ma + 1.f) * 0.5f * n));
    std::memcpy(out, &cube.faces[face][(y * n + x) * 3], 3);
}

/* Sphere map for a camera: texel (s, t) belongs to the eye-space normal
 * (2s - 1, 1 - 2t, z), the view looks down -Z. The reflected directions per
 * texel only depend on that, so they are worked out once; texels outside
 * the sphere get a zero direction. In GX texel order (4x4 tiles). */
inline std::vector<glm::vec3> eye_directions(u32 size)
{
    std::vector<glm::vec3> out;
    out.reserve(size * size);
    for(u32 ty = 0; ty < size; ty += 4)
        for(u32 tx = 0; tx < size; tx += 4)
            for(u32 ry = 0; ry < 4; ry++)
                for(u32 rx = 0; rx < 4; rx++)
                {
                    f32 const nx = (tx + rx + 0.5f) / size * 2.f - 1.f;
                    f32 const ny = 1.f - (ty + ry + 0.5f) / size * 2.f;
                    f32 const r2 = nx * nx + ny * ny;
                    if(r2 >= 1.f)
                    {
                        out.push_back(glm::vec3{0.f});
                        continue;
                    }
                    glm::vec3 const n{nx, ny, std::sqrt(1.f - r2)};
                    out.push_back(glm::vec3{0, 0, -1} + 2.f * n.z * n);
                }
    return out;
}

/* Written as GX RGB565, big-endian. `view` is the camera's world-to-eye
 * rotation, `dirs` from eye_directions() */
inline void bake(
    cube_t const&                 cube,
    glm::mat3 const&              view,
    std::vector<glm::vec3> const& dirs,
    u16*                          dst)
{
    glm::mat3 const to_world = glm::transpose(view);
    for(u32 i = 0; i < dirs.size(); i++)
    {
        u8 rgb[3] = {0, 0, 0};
        if(dirs[i].z != 0.f || dirs[i].x != 0.f || dirs[i].y != 0.f)
            sample(cube, to_world * dirs[i], rgb);
        dst[i] = static_cast<u16>(
            ((rgb[0] >> 3) << 11) | ((rgb[1] >> 2) << 5) | (rgb[2] >> 3));
    }
}

/* Flat water's view of the sky: texel (s, t) holds the world direction r with
 * (r.x, r.y) / r.z over [-extent, extent], weighted from `steep` (looking
 * down) to `grazing` (along the surface). Fixed, so baked once. GX tiled
 * RGB565, big-endian. */
inline void bake_hemisphere(
    cube_t const&    cube,
    u32              size,
    f32              extent,
    glm::vec3 const& steep,
    glm::vec3 const& grazing,
    u16*             dst)
{
    u32 i = 0;
    for(u32 ty = 0; ty < size; ty += 4)
        for(u32 tx = 0; tx < size; tx += 4)
            for(u32 ry = 0; ry < 4; ry++)
                for(u32 rx = 0; rx < 4; rx++)
                {
                    f32 const a =
                        ((tx + rx + 0.5f) / size * 2.f - 1.f) * extent;
                    f32 const b =
                        ((ty + ry + 0.5f) / size * 2.f - 1.f) * extent;
                    glm::vec3 const r = glm::normalize(glm::vec3{a, b, 1.f});
                    u8              rgb[3];
                    sample(cube, r, rgb);
                    f32 const       g  = (1.f - r.z) * (1.f - r.z);
                    glm::vec3 const w  = glm::mix(steep, grazing, g);
                    auto            ch = [&](u32 c) {
                        return static_cast<u32>(
                            std::clamp(rgb[c] * w[c], 0.f, 255.f));
                    };
                    dst[i++] = static_cast<u16>(
                        ((ch(0) >> 3) << 11) | ((ch(1) >> 2) << 5) |
                        (ch(2) >> 3));
                }
}

/* Texture matrix for bake_hemisphere(), from world positions below `eye`: the
 * reflected view direction r = (p.x - e.x, p.y - e.y, e.z - p.z) divided
 * through by r.z, which GX does with q */
inline glm::mat4 hemisphere_texgen(glm::vec3 const& eye, f32 extent)
{
    f32 const k = 0.5f / extent;
    glm::mat4 m{0.f};
    // Column-major: m[column][row]
    m[0][0] = k;
    m[1][1] = k;
    m[2][0] = -0.5f;
    m[2][1] = -0.5f;
    m[2][2] = -1.f;
    m[3][0] = 0.5f * eye.z - k * eye.x;
    m[3][1] = 0.5f * eye.z - k * eye.y;
    m[3][2] = eye.z;
    m[3][3] = 1.f;
    return m;
}

/* Texture matrix taking object normals to sphere map coordinates */
inline glm::mat4 texgen(glm::mat4 const& modelview)
{
    glm::mat4 bias{1.f};
    bias[0][0] = 0.5f;
    bias[1][1] = -0.5f;
    bias[3][0] = 0.5f;
    bias[3][1] = 0.5f;
    return bias * glm::mat4(glm::mat3(modelview));
}

} // namespace gx_reflection
