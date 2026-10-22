#pragma once

#include "glw/enums/FramebufferAttachment.h"
#include "glw/enums/FramebufferTarget.h"
#include "glw/enums/GetPName.h"
#include "glw/enums/InternalFormat.h"
#include "glw/enums/RenderbufferTarget.h"
#include "peripherals/concepts/graphics_api.h"
#include "peripherals/semantic/chunk.h"
#include "peripherals/typing/enum/pixels/format_transform.h"
#include "rhi_features.h"
#include "rhi_texture.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include <glw/extensions/EXT_discard_framebuffer.h>

namespace gleam {

struct rendertarget_t;

namespace detail {
void rendertarget_copy(
    features::rendertargets&    features,
    rendertarget_t&             source,
    rendertarget_t&             dest,
    typing::geometry::rect<u32> srcRect,
    Vecui2                      dstCoord,
    render_targets::attachment  srcAttachment,
    render_targets::attachment  dstAttachment,
    u32                         level,
    u32                         srci,
    u32                         dsti);
} // namespace detail

struct rendertarget_currency
{
    u32 read{0}, draw{0};

    inline bool update(u32 hnd, group::framebuffer_target target)
    {
        bool changed = false;
        switch(target)
        {
        case group::framebuffer_target::framebuffer:
            if(hnd == read && hnd == draw)
                break;
            read = draw = hnd;
            changed     = true;
            break;
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        case group::framebuffer_target::draw_framebuffer:
            if(hnd == draw)
                break;
            draw    = hnd;
            changed = true;
            break;
        case group::framebuffer_target::read_framebuffer:
            if(hnd == read)
                break;
            read    = hnd;
            changed = true;
            break;
#endif
        default:
            Throw(unimplemented_path("read/draw bindings not available"));
        }
        return changed;
    }
};

struct rendertarget_t
{
    static constexpr auto debug_identifier =
        group::object_identifier::framebuffer;

    rendertarget_t(features const& features, rendertarget_currency& currency)
        : m_features(features.rendertarget)
        , m_texture_features(features.texture)
        , m_currency(&currency)
    {
    }

    void alloc()
    {
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
            cmd::create_framebuffers(SpanOne<u32>(m_handle));
        else
#endif
            cmd::gen_framebuffers(SpanOne<u32>(m_handle));
    }

    void dealloc()
    {
        cmd::delete_framebuffers(SpanOne<u32>(m_handle));
        m_handle.release();

        for(auto& [_, buf] : m_renderbufs)
            cmd::delete_renderbuffers(SpanOne(buf.handle.hnd));
    }

    void attach(
        render_targets::attachment attachment,
        texture_t&                 texture,
        u32                        level,
        u32                        i = 0,
        u32 /*layer*/                = 0)
    {
        auto target = internal_collapse_target(
#if GLEAM_MAX_VERSION > 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            group::framebuffer_target::draw_framebuffer
#endif
        );

        if(!m_features.dsa)
            internal_bind(target);
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
            cmd::named_framebuffer_texture(
                m_handle, convert::to(attachment, i), texture.m_handle, level);
        else
#endif
#if GLEAM_MAX_VERSION >= 0x320 || GLEAM_MAX_VERSION_ES >= 0x320
            if(m_features.framebuffer_texture)
        {
            cmd::framebuffer_texture(
                target, convert::to(attachment, i), texture.m_handle, level);
        } else
#endif
        {
            cmd::framebuffer_texture_2d(
                target,
                convert::to(attachment, i),
                convert::to(texture.m_type),
                texture.m_handle,
                level);
        }
    }

    void attach_renderbuffer(
        render_targets::attachment attachment, PixDesc const& fmt)
    {
        auto target = internal_collapse_target(
#if GLEAM_MAX_VERSION > 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            group::framebuffer_target::draw_framebuffer
#endif
        );

        auto& buf = m_renderbufs[attachment];
        buf.fmt   = fmt;
        cmd::gen_renderbuffers(semantic::SpanOne(buf.handle.hnd));
        cmd::bind_renderbuffer(
            group::renderbuffer_target::renderbuffer, buf.handle);
        cmd::bind_renderbuffer(group::renderbuffer_target::renderbuffer, 0);
        if(!m_features.dsa)
            internal_bind(target);
        auto fb_attachment = [&] {
            switch(attachment)
            {
            case render_targets::attachment::depth:
                return group::framebuffer_attachment::depth_attachment;
            case render_targets::attachment::depth_stencil:
                return group::framebuffer_attachment::depth_stencil_attachment;
            default:
                // TODO: Fix multi-level color attachment
                return group::framebuffer_attachment::color_attachment0;
            }
        }();
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
            cmd::named_framebuffer_renderbuffer(
                m_handle,
                fb_attachment,
                group::renderbuffer_target::renderbuffer,
                buf.handle);
        else
#endif
            cmd::framebuffer_renderbuffer(
                target,
                fb_attachment,
                group::renderbuffer_target::renderbuffer,
                buf.handle);
    }

    void copy(
        rendertarget_t&             source,
        typing::geometry::rect<u32> srcRect,
        Vecui2                      dstCoord,
        render_targets::attachment  srcAttachment,
        render_targets::attachment  dstAttachment,
        u32                         level = 0,
        u32                         srci  = 0,
        u32                         dsti  = 0)
    {
        detail::rendertarget_copy(
            m_features,
            source,
            *this,
            srcRect,
            dstCoord,
            srcAttachment,
            dstAttachment,
            level,
            srci,
            dsti);
    }

    void resize(typing::geometry::rect<i32> const& size, u32 = 0)
    {
        auto target = internal_collapse_target(
#if GLEAM_MAX_VERSION > 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            group::framebuffer_target::draw_framebuffer
#endif
        );

        internal_bind(target);
        if(m_handle == 0 || m_external)
        {
            cmd::viewport(Veci2{size.x, size.y}, Veci2{size.w, size.h});
            return;
        }
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
        if(m_features.framebuffer_parameter)
        {
            cmd::framebuffer_parameter(
                target,
                group::framebuffer_parameter_name::framebuffer_default_width,
                size.w);
            cmd::framebuffer_parameter(
                target,
                group::framebuffer_parameter_name::framebuffer_default_width,
                size.h);
        }
#endif
        for(auto& [_, buf] : m_renderbufs)
        {
            cmd::bind_renderbuffer(
                group::renderbuffer_target::renderbuffer, buf.handle);
            auto [ifmt, __, ___] = gleam::convert::to<group::internal_format>(
                buf.fmt, m_texture_features);
            cmd::renderbuffer_storage(
                group::renderbuffer_target::renderbuffer,
                ifmt,
                Veci2{size.w, size.h});
            cmd::bind_renderbuffer(group::renderbuffer_target::renderbuffer, 0);
        }
    }

    size_2d<i32> size()
    {
        auto target = internal_collapse_target(
#if GLEAM_MAX_VERSION > 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            group::framebuffer_target::draw_framebuffer
#endif
        );
        size_2d<i32> out;

        if(!m_features.dsa)
            internal_bind(target);
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
        {
            cmd::get_named_framebuffer_parameter(
                m_handle,
                group::get_framebuffer_parameter::framebuffer_default_width,
                SpanOne(out.w));
            cmd::get_named_framebuffer_parameter(
                m_handle,
                group::get_framebuffer_parameter::framebuffer_default_height,
                SpanOne(out.h));
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
            if(m_features.framebuffer_texture)
        {
            cmd::get_framebuffer_parameter(
                target,
                group::get_framebuffer_parameter::framebuffer_default_width,
                SpanOne(out.w));
            cmd::get_framebuffer_parameter(
                target,
                group::get_framebuffer_parameter::framebuffer_default_height,
                SpanOne(out.h));
        } else
#endif
        {
            typing::vector_types::Veci4 viewport;
            cmd::get_integerv(
                group::get_prop::viewport, SpanOne<i32>(viewport));
            out = {viewport[2], viewport[3]};
        }
        return out;
    }

    template<typename vec_4_f32>
    requires semantic::concepts::Vector<vec_4_f32, f32, 4>
    inline void clear(vec_4_f32 const& color, [[maybe_unused]] u32 i = 0)
    {
        auto target = internal_collapse_target(
#if GLEAM_MAX_VERSION > 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            group::framebuffer_target::draw_framebuffer
#endif
        );
        if(!m_features.dsa)
            internal_bind(target);
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
            cmd::clear_named_framebufferfv(
                m_handle,
                group::buffer::color,
                i,
                Span<const f32>(&color[0], 4));
        else
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            if(m_features.clearbuffer)
            cmd::clear_bufferfv(
                group::buffer::color, i, Span<const f32>(&color[0], 4));
        else
#endif
        {
            cmd::clear_color(color);
            cmd::clear(group::clear_buffer_mask::color_buffer_bit);
        }
    }

    inline void clear(f64 depth)
    {
        auto target = internal_collapse_target(
#if GLEAM_MAX_VERSION > 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            group::framebuffer_target::draw_framebuffer
#endif
        );
        if(!m_features.dsa)
            internal_bind(target);
        auto depthf = static_cast<f32>(depth);
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
            cmd::clear_named_framebufferfv(
                m_handle, group::buffer::depth, 0, SpanOne(depthf));
        else
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            if(m_features.clearbuffer)
            cmd::clear_bufferfv(group::buffer::depth, 0, SpanOne(depthf));
        else
#endif
        {
#if GLEAM_MAX_VERSION_ES > 0
            cmd::clear_depthf(depthf);
#else
            cmd::clear_depth(depth);
#endif
            cmd::clear(group::clear_buffer_mask::depth_buffer_bit);
        }
    }

    inline void clear(i32 stencil)
    {
        auto target = internal_collapse_target(
#if GLEAM_MAX_VERSION > 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            group::framebuffer_target::draw_framebuffer
#endif
        );
        if(!m_features.dsa)
            internal_bind(target);
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
            cmd::clear_named_framebufferiv(
                m_handle, group::buffer::stencil, 0, SpanOne(stencil));
        else
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            if(m_features.clearbuffer)
            cmd::clear_bufferiv(group::buffer::stencil, 0, SpanOne(stencil));
        else
#endif
        {
            cmd::clear_stencil(stencil);
            cmd::clear(group::clear_buffer_mask::stencil_buffer_bit);
        }
    }

    template<typename vec_4_f32>
    // clang-format off
    requires(
        semantic::concepts::Vector<vec_4_f32, f32, 4>
    )
    inline void clear(
        // clang-format on
        vec_4_f32 const&     color,
        f64                  depth,
        i32                  stencil,
        [[maybe_unused]] u32 i = 0)
    {
        auto target = internal_collapse_target(
#if GLEAM_MAX_VERSION > 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            group::framebuffer_target::draw_framebuffer
#endif
        );
        if(!m_features.dsa)
            internal_bind(target);
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
        {
            cmd::clear_named_framebufferfv(
                m_handle,
                group::buffer::color,
                i,
                Span<const f32>(&color[0], 4));
            cmd::clear_named_framebufferfi(
                m_handle,
                static_cast<group::buffer>(GL_DEPTH_STENCIL),
                0,
                depth,
                stencil);
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            if(m_features.clearbuffer)
        {
            cmd::clear_bufferfv(
                group::buffer::color, i, Span<const f32>(&color[0], 4));
            cmd::clear_bufferfi(
                static_cast<group::buffer>(GL_DEPTH_STENCIL),
                0,
                depth,
                stencil);
        } else
#endif
        {
            using group::clear_buffer_mask;

#if GLEAM_MAX_VERSION_ES > 0
            cmd::clear_depthf(static_cast<f32>(depth));
#else
            cmd::clear_depth(depth);
#endif
            cmd::clear_stencil(stencil);
            cmd::clear_color(color);
            cmd::clear(
                clear_buffer_mask::color_buffer_bit |
                clear_buffer_mask::depth_buffer_bit |
                clear_buffer_mask::stencil_buffer_bit);
        }
    }

    inline void discard()
    {
#if GLEAM_MAX_VERSION == 0 && defined(GL_EXT_discard_framebuffer)
        if(m_features.ext.discard_framebuffer)
        {
            using attachment = gl::group::invalidate_framebuffer_attachment;
            std::array<attachment, 3> buffers = {{
                attachment::color_ext,
                attachment::depth_ext,
                attachment::stencil_ext,
            }};
#if GLEAM_MAX_VERSION_ES >= 0x300
            auto target = group::framebuffer_target::draw_framebuffer;
#else
            auto target = group::framebuffer_target::framebuffer;
#endif
            internal_bind(target);
            gl::ext::discard_framebuffer::discard_framebuffer(target, buffers);
        }
#endif
    }

    features::rendertargets m_features;
    features::textures      m_texture_features;
    rendertarget_currency*  m_currency;
    hnd                     m_handle;
    bool m_external{false}; /*!< Owned by the platform, like framebuffer 0 */

    struct renderbuf_t
    {
        hnd     handle;
        PixDesc fmt;
    };

    std::map<render_targets::attachment, renderbuf_t> m_renderbufs;

    void internal_bind(group::framebuffer_target target)
    {
        if(m_currency->update(m_handle, target))
            cmd::bind_framebuffer(target, m_handle);
    }

    group::framebuffer_target internal_collapse_target(
        group::framebuffer_target target =
            group::framebuffer_target::framebuffer)
    {
        if(m_features.readdraw_buffers)
            return target;
        else
            return group::framebuffer_target::framebuffer;
    }
};

inline void detail::rendertarget_copy(
    [[maybe_unused]] features::rendertargets& features,
    [[maybe_unused]] rendertarget_t&          source,
    [[maybe_unused]] rendertarget_t&          dest,
    [[maybe_unused]] typing::geometry::rect<u32> srcRect,
    [[maybe_unused]] Vecui2                      dstCoord,
    [[maybe_unused]] render_targets::attachment  srcAttachment,
    render_targets::attachment /*dstAttachment*/,
    u32 /*level*/,
    u32 /*srci*/,
    u32 /*dsti*/)
{
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
    /* A blit, unscaled: the attachments must share a format */
    if(!features.readdraw_buffers)
        return;
    using mask_t      = group::clear_buffer_mask;
    mask_t const mask = [srcAttachment] {
        switch(srcAttachment)
        {
        case render_targets::attachment::depth:
            return mask_t::depth_buffer_bit;
        case render_targets::attachment::depth_stencil:
            return mask_t::depth_buffer_bit | mask_t::stencil_buffer_bit;
        default:
            return mask_t::color_buffer_bit;
        }
    }();
    /* Bound outright: not every path keeps the currency up to date */
    cmd::bind_framebuffer(group::framebuffer_target::read_framebuffer, source.m_handle);
    cmd::bind_framebuffer(group::framebuffer_target::draw_framebuffer, dest.m_handle);
    source.m_currency->update(source.m_handle, group::framebuffer_target::read_framebuffer);
    dest.m_currency->update(dest.m_handle, group::framebuffer_target::draw_framebuffer);
    auto const x  = static_cast<i32>(srcRect.x);
    auto const y  = static_cast<i32>(srcRect.y);
    auto const w  = static_cast<i32>(srcRect.w);
    auto const h  = static_cast<i32>(srcRect.h);
    auto const dx = static_cast<i32>(dstCoord.x);
    auto const dy = static_cast<i32>(dstCoord.y);
    cmd::blit_framebuffer(
        x,
        y,
        x + w,
        y + h,
        dx,
        dy,
        dx + w,
        dy + h,
        mask,
        group::blit_framebuffer_filter::nearest);
#endif
}

} // namespace gleam
