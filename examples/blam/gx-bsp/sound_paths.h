#pragma once

/* How the structure stands between a sound and its listener, as BlamGraphics
 * hears it: a fan of rays for how blocked the sound is, and the cluster
 * portals for which way it comes round. */

#include <blam/common/collision_bsp.h>
#include <blam/volta/blam_bsp_structures.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <array>
#include <limits>
#include <optional>
#include <vector>

namespace gx_audio {

using libc_types::f32;
using libc_types::i16;
using libc_types::u32;

struct portal_t
{
    i16       front, back; // clusters
    glm::vec3 centroid;
};

class paths_t
{
  public:
    using cluster_fn = std::optional<u32> (*)(glm::vec3 const&);

    struct route_t
    {
        glm::vec3 through; // the portal it reaches the listener's cluster by
        f32       length;
    };

    void set(
        std::vector<blam::collision::bsp_3d> const& nodes,
        std::vector<blam::collision::plane> const&  planes,
        cluster_fn                                  cluster_of)
    {
        m_nodes      = {nodes.data(), nodes.size()};
        m_planes     = {planes.data(), planes.size()};
        m_cluster_of = cluster_of;
    }

    void add_portal(portal_t const& portal)
    {
        m_portals.push_back(portal);
        m_clusters =
            std::max<u32>(m_clusters, std::max(portal.front, portal.back) + 1);
    }

    bool hears_from(glm::vec3 const& listener) const
    {
        return m_cluster_of && m_cluster_of(listener).has_value();
    }

    /* Sounds tend to sit on a floor or inside a wall: somewhere near that
     * is open air, if there is one */
    std::optional<glm::vec3> open_near(
        glm::vec3 const& at, glm::vec3 const& listener) const
    {
        if(m_cluster_of(at))
            return at;
        f32 const       span = glm::length(listener - at);
        glm::vec3 const dir =
            span > 1e-3f ? (listener - at) / span : glm::vec3{0, 0, 1};
        for(f32 reach : {.1f, .3f, .6f, 1.f})
            for(auto const& way : {dir, glm::vec3{0, 0, 1}})
                if(m_cluster_of(at + way * reach))
                    return at + way * reach;
        return std::nullopt;
    }

    /* The share of rays the structure blocks, from a fan around both ends */
    f32 occlusion(glm::vec3 const& listener, glm::vec3 const& source) const
    {
        glm::vec3 const to_source = source - listener;
        f32 const       distance  = glm::length(to_source);
        if(distance < .2f)
            return 0.f;
        glm::vec3 const dir   = to_source / distance;
        glm::vec3       right = glm::cross(dir, glm::vec3{0.f, 0.f, 1.f});
        right = glm::dot(right, right) < 1e-4f ? glm::vec3{1.f, 0.f, 0.f}
                                               : glm::normalize(right);
        glm::vec3 const up = glm::cross(right, dir);

        constexpr f32 source_spread   = .3f;
        constexpr f32 listener_spread = .15f;

        std::array<glm::vec3, 4> const offsets{right, -right, up, -up};

        u32  blocked = 0, total = 0;
        auto cast = [&](glm::vec3 const& from, glm::vec3 const& to) {
            // An end inside solid says nothing about the path between
            if(!m_cluster_of(from) || !m_cluster_of(to))
                return;
            glm::vec3 const span   = to - from;
            f32 const       length = glm::length(span);
            if(length < .2f)
                return;
            total++;
            // Stop short, of the floor or wall the sound sits on
            if(blam::collision_bsp::raycast<blam::collision::traits>(
                   m_nodes,
                   m_planes,
                   from,
                   from + span * ((length - .1f) / length)))
                blocked++;
        };
        cast(listener, source);
        for(auto const& offset : offsets)
            cast(listener, source + offset * source_spread);
        for(auto const& offset : offsets)
            cast(listener + offset * listener_spread, source);
        return total ? static_cast<f32>(blocked) / static_cast<f32>(total)
                     : 0.f;
    }

    /* The shortest way through portals, from their centroids; none if both
     * ends share a cluster or nothing connects them */
    std::optional<route_t> route(
        glm::vec3 const& source, glm::vec3 const& listener) const
    {
        auto const from = m_cluster_of(source);
        auto const to   = m_cluster_of(listener);
        if(!from || !to || *from == *to || *from >= m_clusters ||
           *to >= m_clusters)
            return std::nullopt;

        constexpr f32 unreached = std::numeric_limits<f32>::max();
        m_distance.assign(m_clusters, unreached);
        m_entry.assign(m_clusters, source);
        m_done.assign(m_clusters, 0);
        m_distance[*from] = 0.f;
        for(;;)
        {
            u32 at   = m_clusters;
            f32 best = unreached;
            for(u32 c = 0; c < m_clusters; c++)
                if(!m_done[c] && m_distance[c] < best)
                {
                    best = m_distance[c];
                    at   = c;
                }
            if(at == m_clusters || at == *to)
                break;
            m_done[at] = 1;
            for(auto const& p : m_portals)
            {
                i32 const next = p.front == static_cast<i32>(at)  ? p.back
                                 : p.back == static_cast<i32>(at) ? p.front
                                                                  : -1;
                if(next < 0 || m_done[next])
                    continue;
                f32 const length =
                    best + glm::distance(m_entry[at], p.centroid);
                if(length < m_distance[next])
                {
                    m_distance[next] = length;
                    m_entry[next]    = p.centroid;
                }
            }
        }
        if(m_distance[*to] == unreached)
            return std::nullopt;
        return route_t{
            m_entry[*to],
            m_distance[*to] + glm::distance(m_entry[*to], listener)};
    }

  private:
    using i32 = libc_types::i32;
    using u8  = libc_types::u8;

    semantic::Span<blam::collision::bsp_3d const> m_nodes;
    semantic::Span<blam::collision::plane const>  m_planes;
    cluster_fn                                    m_cluster_of{nullptr};
    std::vector<portal_t>                         m_portals;
    u32                                           m_clusters{0};

    // Kept between routes, so finding one doesn't allocate
    mutable std::vector<f32>       m_distance;
    mutable std::vector<glm::vec3> m_entry;
    mutable std::vector<u8>        m_done;
};

} // namespace gx_audio
