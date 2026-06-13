#pragma once

#include "components.h"

struct DebugMarkers : compo::SubsystemBase
{
    using type = DebugMarkers;

    Span<Vecf3> portal_buffer;
    Span<Vecf3> portal_color_buffer;
    u32         portal_ptr{0};
    u32         portal_color_ptr{0};

    DebugDraw create_box(Vecf3 const& a, Vecf3 const& b, Vecf3 const& color)
    {
        Vecf3 lo = glm::min(a, b);
        Vecf3 hi = glm::max(a, b);
        return create_marker(
            std::array<Vecf3, 16>{{
                lo,
                Vecf3(hi.x, lo.y, lo.z),
                Vecf3(hi.x, hi.y, lo.z),
                Vecf3(lo.x, hi.y, lo.z),
                lo,
                Vecf3(lo.x, lo.y, hi.z),
                Vecf3(hi.x, lo.y, hi.z),
                hi,
                Vecf3(lo.x, hi.y, hi.z),
                Vecf3(lo.x, lo.y, hi.z),
                Vecf3(lo.x, hi.y, hi.z),
                Vecf3(lo.x, hi.y, lo.z),
                Vecf3(hi.x, hi.y, lo.z),
                hi,
                Vecf3(hi.x, lo.y, hi.z),
                Vecf3(hi.x, lo.y, lo.z),
            }},
            color);
    }

    /* Runtime-sized variant of create_marker: writes a line strip of
     * points.size() vertices. Used for collision-mesh surface loops whose
     * vertex count is not known at compile time. */
    DebugDraw create_loop(
        semantic::Span<Vecf3 const> points, Vecf3 const& color)
    {
        u32 n = static_cast<u32>(points.size());
        if(n == 0 || portal_ptr + n > portal_buffer.size() ||
           portal_color_ptr >= portal_color_buffer.size())
        {
            cWarning("DebugMarkers: portal buffer full, dropping loop");
            return {};
        }
        auto verts = portal_buffer.subspan(portal_ptr, n);
        std::copy(points.begin(), points.end(), verts.begin());
        portal_color_buffer[portal_color_ptr] = color;
        DebugDraw draw                        = {
                                   .data =
                {
                                           .arrays =
                        {
                                                   .count  = n,
                                                   .offset = portal_ptr,
                        },
                },
                                   .color_ptr = portal_color_ptr,
        };
        portal_ptr += n;
        portal_color_ptr++;
        return draw;
    }

    template<size_t N>
    DebugDraw create_marker(
        std::array<Vecf3, N> const& points, Vecf3 const& color)
    {
        if(portal_ptr + N > portal_buffer.size() ||
           portal_color_ptr >= portal_color_buffer.size())
        {
            cWarning("DebugMarkers: portal buffer full, dropping marker");
            return {};
        }
        auto verts = portal_buffer.subspan(portal_ptr, N);
        std::copy(points.begin(), points.end(), verts.begin());
        portal_color_buffer[portal_color_ptr] = color;
        DebugDraw draw                        = {
                                   .data =
                {
                                           .arrays =
                        {
                                                   .count  = static_cast<u32>(N),
                                                   .offset = portal_ptr,
                        },
                },
                                   .color_ptr = portal_color_ptr,
        };
        portal_ptr += static_cast<u32>(N);
        portal_color_ptr++;
        return draw;
    }
};
