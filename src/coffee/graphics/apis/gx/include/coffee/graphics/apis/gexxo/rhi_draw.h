#pragma once

#include "peripherals/enum/helpers.h"
#include "rhi_resources.h"

#include <array>
#include <ogc/cache.h>
#include <ogc/gx.h>
#include <optional>
#include <peripherals/concepts/graphics_api.h>
#include <peripherals/semantic/enum/data_types.h>
#include <peripherals/typing/enum/pixels/components.h>
#include <tuple>

namespace gexxo {
namespace detail {
inline void glm_to_mtx(Matf4 const& m, Mtx out)
{
    auto const* g = reinterpret_cast<f32 const*>(&m);
    for(int r = 0; r < 3; r++)
        for(int c = 0; c < 4; c++)
            out[r][c] = g[c * 4 + r];
}
inline u32 type_size(semantic::type_t t)
{
    switch(t)
    {
    case semantic::type_t::u8:
    case semantic::type_t::i8: return 1;
    case semantic::type_t::u16:
    case semantic::type_t::i16: return 2;
    default: return 4; /* u32/i32/f32 */
    }
}

/* Rough hash of the parts of a draw that determine the recorded GX command
 * stream: the vertex array (its attribute layout + bound buffers, by identity),
 * the element buffer, and the draw counts/offsets/mode. The per-instance matrix
 * and render state are NOT included -- they are applied outside the list. Buffer
 * identity (pointer + size) stands in for content: rebinding or resizing a
 * buffer changes the key; mutating a buffer in place without changing its
 * pointer would not (acceptable for the static geometry this caches). */
inline u64 hash_draw(draw_command const& cmd)
{
    u64  h   = 1469598103934665603ull; /* FNV-1a */
    auto mix = [&h](u64 v) { h = (h ^ v) * 1099511628211ull; };

    mix(static_cast<u64>(reinterpret_cast<uintptr_t>(cmd.vertices.get())));
    mix(cmd.call.indexed ? 1u : 0u);
    mix(static_cast<u64>(cmd.call.mode));

    if(cmd.call.indexed)
    {
        mix(cmd.data.elements.count);
        mix(cmd.data.elements.offset);
        mix(static_cast<u64>(cmd.data.elements.type));
        if(auto const& eb = cmd.vertices->m_element_buffer; eb)
        {
            mix(reinterpret_cast<uintptr_t>(eb.get()));
            mix(eb->size());
        }
    } else
    {
        mix(cmd.data.arrays.count);
        mix(cmd.data.arrays.offset);
    }

    for(vertex_attribute const& a : cmd.vertices->m_attributes)
    {
        mix(static_cast<u64>(a.role));
        mix(a.value.offset);
        mix(a.value.stride);
        mix(a.value.count);
        mix(static_cast<u64>(a.value.type));
        mix(a.buffer.id);
        mix(a.buffer.offset);
    }
    for(auto const& [id, buf] : cmd.vertices->m_vertex_buffers)
    {
        mix(id);
        mix(buf ? reinterpret_cast<uintptr_t>(buf.get()) : 0);
        mix(buf ? buf->size() : 0);
    }
    return h;
}

inline u8 prim_to_gx(drawing::primitive prim)
{
    switch(prim)
    {
    case drawing::primitive::point: return GX_POINTS;

    case drawing::primitive::line: return GX_LINES;
    case drawing::primitive::line_strip: return GX_LINESTRIP;

    case drawing::primitive::triangle_fan: return GX_TRIANGLEFAN;
    case drawing::primitive::triangle_strip: return GX_TRIANGLESTRIP;
    case drawing::primitive::triangle: return GX_TRIANGLES;
    default: Throw(std::out_of_range("invalid primitive mode for GX"));
    }
}

inline void iterate_indices(
    draw_command::data_t const& data,
    vertex_array_t const& array)
{
    if(!array.m_element_buffer)
        Throw(std::out_of_range("indexed draw: no element buffer bound"));

    if(data.elements.type == semantic::type_t::u16)
    {
        gsl::span<u16> indices = array.m_element_buffer->map<u16>(
            data.elements.offset * sizeof(u16),
            data.elements.count * sizeof(u16));
        for(u16 idx : indices)
            for(vertex_attribute const& attr : array.m_attributes)
                ::wgPipe->U16 = idx;
    } else {
        gsl::span<u8> indices = array.m_element_buffer->map<u8>(
            data.elements.offset,
            data.elements.count);
        for(u8 idx : indices)
            for(vertex_attribute const& attr : array.m_attributes)
                ::wgPipe->U8 = idx;
    }
}

inline void iterate_arrays(
    draw_command::data_t const& data,
    vertex_array_t const& array)
{
    using type_t = semantic::type_t;

    std::array<libc_types::u8 const*, 30> buffers{};
    for(auto const& [idx, buf] : array.m_vertex_buffers)
    {
        buffers[idx] = buf->map<libc_types::u8 const>().data();
    }
    auto vtx_write = [&buffers]<typename T, typename WG>(
        vertex_attribute const& attr, size_t vtxoffset, WG& out)
    {
        auto ptr = reinterpret_cast<T const*>(
            buffers[attr.buffer.id] +
            attr.buffer.offset +
            attr.value.offset +
            // Vertex striding happens here
            vtxoffset * attr.value.stride);
        // Each store to the write-gather pipe (wgPipe) pushes one component into
        // the GP FIFO, in the attribute's configured component count.
        for(size_t i = 0; i < attr.value.count; i++)
            out = ptr[i];
    };
    for(size_t i = data.arrays.offset; i<data.arrays.offset + data.arrays.count; i++)
    {
        for(vertex_attribute const& attr : array.m_attributes)
        {
            switch(attr.role)
            {
            case vertex_attribute::color0:
            case vertex_attribute::color1:
                switch(attr.value.type)
                {
                case semantic::type_t::f32:
                {
                    f32 const* ptr = reinterpret_cast<f32 const*>(buffers[attr.buffer.id] +
                        attr.buffer.offset +
                        attr.value.offset +
                        i * attr.value.stride);
                    // f32 color is not really supported
                    for(size_t i=0; i<attr.value.count; i++)
                        ::wgPipe->U8 = u8(ptr[i] * 255.f);
                    break;
                }
                case semantic::type_t::u8:
                {
                    u8 const* ptr = buffers[attr.buffer.id] +
                        attr.buffer.offset +
                        attr.value.offset +
                        i * attr.value.stride;
                    for(size_t i=0; i<attr.value.count; i++)
                        ::wgPipe->U8 = ptr[i];
                    break;
                }
                case semantic::type_t::u16:
                {
                    u16 const* ptr = reinterpret_cast<u16 const*>(buffers[attr.buffer.id] +
                        attr.buffer.offset +
                        attr.value.offset +
                        i * attr.value.stride);
                    ::wgPipe->U16 = ptr[0];
                    break;
                }
                case semantic::type_t::u32:
                {
                    u32 const* ptr = reinterpret_cast<u32 const*>(buffers[attr.buffer.id] +
                        attr.buffer.offset +
                        attr.value.offset +
                        i * attr.value.stride);
                    ::wgPipe->U32 = ptr[0];
                    break;
                }
                default:
                    break;
                }
                break;
            default:
                switch(attr.value.type)
                {
                case type_t::u8:
                    vtx_write.template operator()<libc_types::u8>(attr, i, ::wgPipe->U8);
                    break;
                case type_t::i8:
                    vtx_write.template operator()<libc_types::i8>(attr, i, ::wgPipe->S8);
                    break;
                case type_t::u16:
                    vtx_write.template operator()<libc_types::u16>(attr, i, ::wgPipe->U16);
                    break;
                case type_t::i16:
                    vtx_write.template operator()<libc_types::i16>(attr, i, ::wgPipe->S16);
                    break;
                case type_t::f32:
                    vtx_write.template operator()<libc_types::f32>(attr, i, ::wgPipe->F32);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

inline void apply_state(cull_state const& state)
{
    GX_SetCullMode(state.front_face ? GX_CULL_FRONT : GX_CULL_BACK);
}
inline void apply_state(blend_state const& state)
{
    if(state.additive)
        GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_NONE);
    else if(state.multiply)
        GX_SetBlendMode(GX_BM_BLEND, GX_BL_DSTCLR, GX_BL_INVSRCALPHA, GX_NONE);
    else
        GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_NONE);
}
inline void apply_state(view_state const& state)
{
    // GX_SetViewport(
    //     state.view.x, state.view.y,
    //     state.view.z, state.view.w,
    //     state.depth.range.x,
    //     state.depth.range.y);
    // GX_SetScissor(
    //     state.scissor.x, state.scissor.y,
    //     state.scissor.z, state.scissor.w);
}

inline void undo_state(cull_state const& state)
{
    GX_SetCullMode(GX_CULL_BACK);
}
inline void undo_state(view_state const& state)
{
}
inline void undo_state(blend_state const& state)
{
    GX_SetBlendMode(GX_BM_NONE, GX_NONE, GX_NONE, GX_NONE);
}
}

template<typename... StateList>
inline std::optional<std::tuple<error, std::string_view>> api::submit(
        draw_command const& cmd, StateList&&... state_changes)
{
    if(!cmd.vertices || cmd.vertices->m_attributes.empty())
        return std::nullopt;
    auto& vao = *cmd.vertices;

    u32 const count =
        cmd.call.indexed ? cmd.data.elements.count : cmd.data.arrays.count;
    if(count == 0)
        return std::nullopt;

    auto const& xforms        = cmd.instance_transforms;
    bool const  has_transform = !xforms.empty();

    Mtx44 proj;
    if(has_transform)
    {
        // TODO: Make perspective params configurable
        guPerspective(proj, 90.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        GX_LoadProjectionMtx(proj, GX_PERSPECTIVE);
    } else
    {
        guOrtho(proj, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
        GX_LoadProjectionMtx(proj, GX_ORTHOGRAPHIC);
        Mtx mv;
        guMtxIdentity(mv);
        guMtxTransApply(mv, mv, 0.0f, 0.0f, -5.0f);
        GX_LoadPosMtxImm(mv, GX_PNMTX0);
    }

    GX_SetColorUpdate(GX_TRUE);

    GX_SetNumChans(cmd.program->channels.size());
    for(program_t::channel_t const& channel : cmd.program->channels)
        GX_SetChanCtrl(
            channel.channel,
            channel.lighting ? GX_ENABLE : GX_DISABLE,
            channel.ambient_src, channel.diffuse_src,
            channel.light_mask,
            channel.diffuse_function,
            channel.attenuation_function);
    // Textures: bind each to its texture map and generate its coordinates
    // (the generation source/type may vary per texture).
    GX_SetNumTexGens(cmd.textures.size());
    for(texture_binding_t const& tex : cmd.textures)
    {
        if(tex.texture && tex.texture->m_loaded)
            GX_LoadTexObj(&tex.texture->m_obj, tex.texmap);
        GX_SetTexCoordGen(tex.texcoord, tex.gen_type, tex.gen_src, tex.gen_mtx);
    }

    // TEV stages, configured by the program.
    GX_SetNumTevStages(cmd.program->stages.size());
    for(program_t::stage_t const& s : cmd.program->stages)
    {
        GX_SetTevOrder(s.stage, s.texcoord, s.texmap, s.color);
        if(s.op == program_t::stage_t::modulate_prev ||
           s.op == program_t::stage_t::modulate_prev_2x)
        {
            // out = (cprev * texc) [* 2 for the 2x variant] (multi-texture
            // modulate, e.g. diffuse*lightmap, or detail double-biased-multiply)
            u8 const scale = s.op == program_t::stage_t::modulate_prev_2x
                                 ? GX_CS_SCALE_2
                                 : GX_CS_SCALE_1;
            GX_SetTevColorIn(
                s.stage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
            GX_SetTevColorOp(
                s.stage, GX_TEV_ADD, GX_TB_ZERO, scale, GX_TRUE, GX_TEVPREV);
            GX_SetTevAlphaIn(
                s.stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
            GX_SetTevAlphaOp(
                s.stage, GX_TEV_ADD, GX_TB_ZERO, scale, GX_TRUE, GX_TEVPREV);
        } else
            GX_SetTevOp(s.stage, s.op);
    }

    // Default to alpha blending; a blend_state in the state changes overrides it.
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

    /* Describe + format ONLY the configured attributes. Iterating the full
     * m_attr_desc/m_attr_fmt arrays would emit zero-filled entries (attr 0 =
     * GX_VA_PNMTXIDX) and GX_SetVtxAttrFmtv would run past the unterminated list,
     * making the GP expect vertex bytes the draw never emits -> FIFO stall. */
    GX_ClearVtxDesc();
    for(vertex_attribute const& attr : vao.m_attributes)
    {
        u8 const gxattr = detail::role_to_attr(attr);
        GX_SetVtxDesc(gxattr, cmd.call.indexed
            ? (cmd.data.elements.type == semantic::type_t::u16
                ? GX_INDEX16
                : GX_INDEX8)
            : GX_DIRECT);
        GX_SetVtxAttrFmt(
            GX_VTXFMT0,
            gxattr,
            detail::attr_to_type(attr),
            detail::attr_to_fmt(attr),
            0 /*frac*/);
        if(cmd.call.indexed)
        {
            auto buffer = vao.m_vertex_buffers.find(attr.buffer.id);
            if(buffer == vao.m_vertex_buffers.end() || !buffer->second)
                Throw(std::out_of_range("indexed draw: vertex buffer not bound"));
            // The array base must point at THIS attribute's first element, i.e.
            // include its in-vertex offset; the GP then reads base + index*stride.
            auto slice = buffer->second->slice(
                attr.buffer.offset + attr.value.offset).map();
            GX_SetArray(gxattr, slice.data(), attr.value.stride);
        }
    }

    (detail::apply_state(state_changes), ...);

    /* Record the geometry (GX_Begin..vertices..GX_End) into a display list the
     * first time we see this draw, then replay the cached FIFO every frame so
     * only the matrix/material colour are re-issued per instance. The vertex
     * descriptor/format/array set above stay outside the list (they are state,
     * re-applied each submit). */
    u64 const dl_key = detail::hash_draw(cmd);
    auto      dl_it  = m_display_lists.find(dl_key);
    if(dl_it == m_display_lists.end())
    {
        u32 per_vert = 0;
        if(cmd.call.indexed)
        {
            u32 const idxsz =
                cmd.data.elements.type == semantic::type_t::u16 ? 2u : 1u;
            per_vert = static_cast<u32>(vao.m_attributes.size()) * idxsz;
        } else
            for(vertex_attribute const& a : vao.m_attributes)
                per_vert += a.value.count * detail::type_size(a.value.type);

        /* Vertex bytes + GX_Begin/GX_End overhead, padded to a 32-byte multiple
         * (GX requires the list buffer aligned + a multiple of 32 in size). */
        u32 cap = count * per_vert + 64;
        cap     = (cap + 31u) & ~31u;

        void* buf = memalign(32, cap);
        if(!buf)
            return std::make_tuple(
                error::failed,
                std::string_view{"display list alloc failed"});

        GX_BeginDispList(buf, cap);
        GX_Begin(detail::prim_to_gx(cmd.call.mode), GX_VTXFMT0, count);
        if(cmd.call.indexed)
            detail::iterate_indices(cmd.data, vao);
        else
            detail::iterate_arrays(cmd.data, vao);
        GX_End();
        u32 const used = GX_EndDispList();
        DCFlushRange(buf, used); /* make the recorded list visible to the GP */

        dl_it = m_display_lists.emplace(dl_key, display_list_t{buf, used}).first;
    }
    display_list_t const& dl = dl_it->second;

    static const u8 palette[4][3] = {
        {0x40, 0xE0, 0x40}, /* green */
        {0xE0, 0x40, 0x40}, /* red   */
        {0x40, 0x80, 0xE0}, /* blue  */
        {0xE0, 0xE0, 0x40}, /* yellow*/
    };

    for(u32 inst = cmd.data.instances.offset;
        inst < (cmd.call.instanced ? cmd.data.instances.count : 0);
        inst++)
    {
        if(has_transform)
        {
            Mtx mv;
            detail::glm_to_mtx(xforms[inst], mv); /* xforms are modelview already */
            GX_LoadPosMtxImm(mv, GX_PNMTX0);
        }

        /* Per-instance material colour (used when a channel sources GX_SRC_REG;
         * ignored when it sources the vertex colour). Kept outside the list. */
        auto const&   col    = palette[inst % 4];
        GXColor const matcol = {col[0], col[1], col[2], 0xFF};
        GX_SetChanMatColor(GX_COLOR0A0, matcol);

        GX_CallDispList(dl.buffer, dl.size);
    }
    (detail::undo_state(state_changes), ...);
    return std::nullopt;
}

}
