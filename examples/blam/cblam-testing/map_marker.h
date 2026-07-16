#pragma once

#include "components.h"

#include <map>
#include <vector>

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

    /*!
     * \brief Reserve a persistent slot for one line strip (vert_count
     * vertices + one colour), held until release_strip(). Unlike
     * portal_ptr above — an append-only cursor for one-shot, load-time
     * geometry (BSP portal loops, trigger-volume boxes) — a strip slot
     * belongs to its caller for as long as they want it, and is
     * rewritten in place every frame via put_strip(). Each distinct
     * visualization instance (one physics body, one player's eye marker,
     * ...) gets its own slot instead of every instance sharing a single
     * hardcoded offset — that sharing was the actual bug: with a fixed
     * per-subsystem slot, only the last consumer to write in a given
     * frame was visible, and the slot count was a compile-time ceiling
     * unrelated to the buffer's real size.
     */
    struct strip_slot_t
    {
        u32 vert_offset{0};
        u32 vert_count{0};
        u32 color_idx{0};

        bool valid() const
        {
            return vert_count != 0;
        }
    };

    /* Total capacity of the underlying buffers, in Vecf3 elements — known
     * from allocation time (resource_creation.cpp via set_capacity()),
     * independent of whether the buffers are currently mapped. The
     * persistent-strip allocator below grows DOWN from this capacity
     * while the one-shot append cursor (portal_ptr) grows UP from just
     * past the fixed axes prefix, so both share the whole buffer instead
     * of a hand-picked prefix, and each fails gracefully (denies the
     * request, callers already handle that) only once they'd actually
     * meet. */
    u32 vert_capacity{0};
    u32 color_capacity{0};

    void set_capacity(u32 verts, u32 colors)
    {
        vert_capacity       = verts;
        color_capacity      = colors;
        m_strip_vert_floor  = verts;
        m_strip_color_floor = colors;
    }

    strip_slot_t acquire_strip(u32 vert_count)
    {
        if(vert_count == 0)
            return {};

        auto& free_verts  = m_free_strip_verts[vert_count];
        bool  reuse_verts = !free_verts.empty();
        bool  reuse_color = !m_free_strip_colors.empty();

        if(!reuse_verts && vert_count > (m_strip_vert_floor - portal_ptr))
        {
            cWarning(
                "DebugMarkers: strip buffer full, denying slot for {} verts",
                vert_count);
            return {};
        }
        if(!reuse_color && m_strip_color_floor <= portal_color_ptr)
        {
            cWarning("DebugMarkers: strip colour buffer full, denying slot");
            return {};
        }

        u32 vert_offset;
        if(reuse_verts)
        {
            vert_offset = free_verts.back();
            free_verts.pop_back();
        } else
        {
            m_strip_vert_floor -= vert_count;
            vert_offset = m_strip_vert_floor;
        }

        u32 color_idx;
        if(reuse_color)
        {
            color_idx = m_free_strip_colors.back();
            m_free_strip_colors.pop_back();
        } else
        {
            m_strip_color_floor -= 1;
            color_idx = m_strip_color_floor;
        }

        return strip_slot_t{vert_offset, vert_count, color_idx};
    }

    /* Returns a slot to the free list for reuse — call when the owning
     * body/player goes away, otherwise repeated join/leave (netcode
     * testing, physics bodies rebuilding) permanently eats buffer space. */
    void release_strip(strip_slot_t const& slot)
    {
        if(!slot.valid())
            return;
        m_free_strip_verts[slot.vert_count].push_back(slot.vert_offset);
        m_free_strip_colors.push_back(slot.color_idx);
    }

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
        /* Bounded by the strip floor, not raw buffer size: the top of the
         * buffer belongs to acquire_strip()'s persistent slots. */
        if(n == 0 || portal_ptr + n > m_strip_vert_floor ||
           portal_color_ptr >= m_strip_color_floor)
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
        /* Bounded by the strip floor, not raw buffer size: the top of the
         * buffer belongs to acquire_strip()'s persistent slots. */
        if(portal_ptr + N > m_strip_vert_floor ||
           portal_color_ptr >= m_strip_color_floor)
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

    /* Overwrite a slot reserved via acquire_strip() (or, historically, a
     * fixed offset) in place — does not move the append cursor. Buffers
     * must be mapped. Used for per-frame markers (physics bodies, occluder
     * eye markers) whose vertex/colour slots persist across frames and are
     * rewritten every frame rather than re-appended. */
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

  private:
    u32 m_strip_vert_floor{0};
    u32 m_strip_color_floor{0};
    std::map<u32, std::vector<u32>> m_free_strip_verts;
    std::vector<u32>                m_free_strip_colors;
};
