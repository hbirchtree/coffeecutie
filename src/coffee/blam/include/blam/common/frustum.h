#pragma once

#include <peripherals/typing/vectors/vector_types.h>

#include <array>
#include <vector>

namespace blam {

using libc_types::f32;
using typing::vector_types::Matf4;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

/* Camera frustum built from a projection×view matrix (Gribb-Hartmann).
 * Only the four side planes are used; near/far are skipped because the
 * projection matrix in this codebase sets m[2][2]=0 for an infinite far
 * plane, making those two rows unreliable.
 * cam_plane (row3 of MVP) identifies vertices in front of the camera:
 * dot(cam_plane.xyz, v) + cam_plane.w > 0  ↔  clip_w > 0  ↔  in front. */
struct Frustum
{
    std::array<Vecf4, 4> planes;    /* left, right, bottom, top */
    Vecf4                cam_plane; /* row3 of MVP — NOT normalised */

    static Frustum from_mvp(Matf4 const& m)
    {
        auto row = [&](int j) {
            return Vecf4(m[0][j], m[1][j], m[2][j], m[3][j]);
        };
        Vecf4   r0 = row(0), r1 = row(1), r3 = row(3);
        Frustum f;
        f.cam_plane = r3;
        f.planes[0] = r3 + r0; /* left   */
        f.planes[1] = r3 - r0; /* right  */
        f.planes[2] = r3 + r1; /* bottom */
        f.planes[3] = r3 - r1; /* top    */
        for(auto& p : f.planes)
        {
            f32 len = glm::length(Vecf3(p));
            if(len > 0.f)
                p /= len;
        }
        return f;
    }

    /* Returns false if the AABB (bmin..bmax, world space) is entirely behind
     * the camera or entirely outside any of the four side planes.
     * Uses the p-vertex trick: for each plane pick the AABB corner with the
     * highest dot-product against the plane normal; if even that corner is
     * outside, the whole box is. */
    bool aabb_visible(Vecf3 const& bmin, Vecf3 const& bmax) const
    {
        bool cam_valid = glm::dot(Vecf3(cam_plane), Vecf3(cam_plane)) > 1e-10f;
        if(cam_valid)
        {
            Vecf3 p{
                cam_plane.x >= 0.f ? bmax.x : bmin.x,
                cam_plane.y >= 0.f ? bmax.y : bmin.y,
                cam_plane.z >= 0.f ? bmax.z : bmin.z,
            };
            if(glm::dot(Vecf3(cam_plane), p) + cam_plane.w <= 0.f)
                return false;
        }
        for(auto const& plane : planes)
        {
            Vecf3 p{
                plane.x >= 0.f ? bmax.x : bmin.x,
                plane.y >= 0.f ? bmax.y : bmin.y,
                plane.z >= 0.f ? bmax.z : bmin.z,
            };
            if(glm::dot(Vecf3(plane), p) + plane.w < 0.f)
                return false;
        }
        return true;
    }

    /* Returns false if the sphere (center c, radius r, world space) is
     * entirely behind the camera or entirely outside any of the four side
     * planes. Side planes are normalised at construction; cam_plane is not,
     * so it is normalised here for the radius offset to be in world units. */
    bool sphere_visible(Vecf3 const& c, f32 r) const
    {
        f32 cam_len = glm::length(Vecf3(cam_plane));
        if(cam_len > 1e-5f &&
           (glm::dot(Vecf3(cam_plane), c) + cam_plane.w) / cam_len <= -r)
            return false;
        for(auto const& p : planes)
            if(glm::dot(Vecf3(p), c) + p.w < -r)
                return false;
        return true;
    }

    /* Returns false if the polygon is entirely outside any single plane.
     * Vertices behind the camera (clip_w ≤ 0) are treated as "inside" all
     * planes because Gribb-Hartmann gives inverted results for them.
     * If ALL vertices are behind the camera the portal is culled.
     * If cam_plane is degenerate (zero — happens on the first frame before
     * the camera quaternion is initialised) the camera-side test is skipped
     * and we fall back to the plain per-plane polygon test. */
    bool polygon_inside(std::vector<Vecf3> const& verts) const
    {
        if(verts.empty())
            return true;

        bool cam_valid = glm::dot(Vecf3(cam_plane), Vecf3(cam_plane)) > 1e-10f;

        if(cam_valid)
        {
            /* All vertices behind camera → portal is behind us. */
            bool any_front = false;
            for(auto const& v : verts)
            {
                if(glm::dot(Vecf3(cam_plane), v) + cam_plane.w > 0.f)
                {
                    any_front = true;
                    break;
                }
            }
            if(!any_front)
                return false;
        }

        /* Small tolerance so a portal whose vertex grazes the frustum boundary
         * doesn't flicker in/out with minor camera movement. */
        constexpr f32 frustum_eps = 0.1f;

        for(auto const& p : planes)
        {
            bool all_outside = true;
            for(auto const& v : verts)
            {
                /* When cam_plane is valid, behind-camera vertices count as
                 * "inside" — their clip_w < 0 inverts the half-space test. */
                if(cam_valid &&
                   glm::dot(Vecf3(cam_plane), v) + cam_plane.w <= 0.f)
                {
                    all_outside = false;
                    break;
                }
                if(glm::dot(Vecf3(p), v) + p.w >= -frustum_eps)
                {
                    all_outside = false;
                    break;
                }
            }
            if(all_outside)
                return false;
        }
        return true;
    }
};

} // namespace blam
