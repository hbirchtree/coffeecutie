#pragma once

#include "components.h"

/* Single owner of the debug-line vertex/colour buffers and every CPU-side
 * write to them. Components never touch BlamResources::debug_lines or
 * ::debug_line_colors directly — they bind the buffers once, then map(),
 * append via create_* / overwrite fixed slots via put_strip, and unmap()
 * through here. This keeps the map/unmap lifetime and the append cursor in
 * one place instead of spread across loading, occluder, physics and
 * rendering. */
struct DebugMarkers : compo::SubsystemBase
{
    using type = DebugMarkers;

    /* Non-owning handles to the GPU buffers created in resource_creation. */
    std::shared_ptr<gfx::buffer_t> lines;
    std::shared_ptr<gfx::buffer_t> colors;

    Span<Vecf3> portal_buffer;
    Span<Vecf3> portal_color_buffer;
    u32         portal_ptr{0};
    u32         portal_color_ptr{0};

    /* True once the debug-line buffers exist (false on GL ES 2.0 / mobile). */
    /* Wired to RenderingParameters::debug_markers at registration; gating
     * available() here turns off every per-frame marker writer at once —
     * mapping the buffers stalls on GPU sync, so no writer should touch
     * them while markers aren't drawn. One-time creators (load-time axes,
     * boxes) call map() directly and are not affected. */
    bool const* enabled{nullptr};

    bool available() const
    {
        return static_cast<bool>(lines) && (!enabled || *enabled);
    }

    /* Map both buffers for CPU writes and reset the append cursor. */
    void map(u32 vertex_cursor, u32 color_cursor)
    {
        map();
        portal_ptr       = vertex_cursor;
        portal_color_ptr = color_cursor;
    }

    /* Map both buffers without disturbing the cursor (continue appending). */
    void map()
    {
        if(!lines)
            return;
        portal_buffer       = lines->map<Vecf3>(0);
        portal_color_buffer = colors->map<Vecf3>(0);
    }

    void unmap()
    {
        if(!lines)
            return;
        lines->unmap();
        colors->unmap();
        portal_buffer       = {};
        portal_color_buffer = {};
    }

    /* 16-vertex wire box: a line_strip tracing all 12 edges of the AABB
     * spanned by a/b. Shared by create_box and the per-frame probe markers. */
    static std::array<Vecf3, 16> box_vertices(Vecf3 const& a, Vecf3 const& b)
    {
        Vecf3 lo = glm::min(a, b);
        Vecf3 hi = glm::max(a, b);
        return {{
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
        }};
    }

    DebugDraw create_box(Vecf3 const& a, Vecf3 const& b, Vecf3 const& color)
    {
        return create_marker(box_vertices(a, b), color);
    }

    /* Runtime-sized variant of create_marker: writes a line strip of
     * points.size() vertices. Used for collision-mesh surface loops whose
     * vertex count is not known at compile time. */
    DebugDraw create_loop(
        semantic::Span<Vecf3 const> points, Vecf3 const& color)
    {
        if(portal_buffer.empty())
            return {};
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
        if(portal_buffer.empty())
            return {};
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

    /* Overwrite a previously-reserved fixed slot in place — does not move the
     * append cursor. Buffers must be mapped. Used for per-frame markers
     * (physics probe box, occluder eye markers) whose vertex/colour slots are
     * statically reserved and rewritten every frame. */
    void put_strip(
        u32                         vert_offset,
        u32                         color_idx,
        semantic::Span<Vecf3 const> verts,
        Vecf3 const&                color)
    {
        if(portal_buffer.empty() ||
           vert_offset + verts.size() > portal_buffer.size() ||
           color_idx >= portal_color_buffer.size())
            return;
        std::copy(
            verts.begin(), verts.end(), portal_buffer.begin() + vert_offset);
        portal_color_buffer[color_idx] = color;
    }
};
