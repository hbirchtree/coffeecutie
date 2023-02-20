#pragma once

#include "rhi_features.h"
#include "rhi_texture.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include <glw/extensions/EXT_discard_framebuffer.h>

namespace gleam {

struct rendertarget_t;

namespace detail {
void rendertarget_copy(
    features::rendertargets&        features,
    rendertarget_t&                 source,
    rendertarget_t&                 dest,
    typing::geometry::rect<u32>     srcRect,
    typing::geometry::point_2d<u32> dstCoord,
    render_targets::attachment      srcAttachment,
    render_targets::attachment      dstAttachment,
    u32                             level,
    u32                             srci,
    u32                             dsti);
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
    rendertarget_t(
        features::rendertargets const& features,
        rendertarget_currency&         currency) :
        m_features(features),
        m_currency(&currency)
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

    void copy(
        rendertarget_t&                 source,
        typing::geometry::rect<u32>     srcRect,
        typing::geometry::point_2d<u32> dstCoord,
        render_targets::attachment      srcAttachment,
        render_targets::attachment      dstAttachment,
        u32                             level = 0,
        u32                             srci  = 0,
        u32                             dsti  = 0)
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
        if(m_handle == 0)
        {
            cmd::viewport(Veci2{size.x, size.y}, Veci2{size.w, size.h});
            return;
        }
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
        cmd::framebuffer_parameter(
            target,
            group::framebuffer_parameter_name::framebuffer_default_width,
            size.w);
        cmd::framebuffer_parameter(
            target,
            group::framebuffer_parameter_name::framebuffer_default_width,
            size.h);
#endif
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
#if GLEAM_MAX_VERSION >= 0x320 || GLEAM_MAX_VERSION_ES >= 0x320
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
            out = {viewport.z(), viewport.w()};
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
                clear_buffer_mask::color_buffer_bit
                | clear_buffer_mask::depth_buffer_bit
                | clear_buffer_mask::stencil_buffer_bit);
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
    rendertarget_currency*  m_currency;
    hnd                     m_handle;

    void internal_bind(group::framebuffer_target target)
    {
        if(m_currency->update(m_handle, target))
            cmd::bind_framebuffer(target, m_handle);
    }

    group::framebuffer_target internal_collapse_target(
        group::framebuffer_target target
        = group::framebuffer_target::framebuffer)
    {
        if(m_features.readdraw_buffers)
            return target;
        else
            return group::framebuffer_target::framebuffer;
    }
};

inline void detail::rendertarget_copy(
    [[maybe_unused]] features::rendertargets&   features,
    rendertarget_t&                             source,
    rendertarget_t&                             dest,
    typing::geometry::rect<u32>                 srcRect,
    typing::geometry::point_2d<u32>             dstCoord,
    [[maybe_unused]] render_targets::attachment srcAttachment,
    render_targets::attachment                  dstAttachment,
    u32                                         level,
    [[maybe_unused]] u32                        srci,
    u32                                         dsti)
{
    using param = group::framebuffer_attachment_parameter_name;

    i32 dstTexture = 0;
    i32 dstTarget  = 0;
#if GLEAM_MAX_VERSION >= 0x450
    if(features.dsa)
    {
        cmd::get_named_framebuffer_attachment_parameter(
            dest.m_handle,
            convert::to(dstAttachment, dsti),
            param::framebuffer_attachment_object_name,
            SpanOne(dstTexture));
        cmd::get_named_framebuffer_attachment_parameter(
            dest.m_handle,
            convert::to(dstAttachment, dsti),
            param::framebuffer_attachment_texture_cube_map_face,
            SpanOne(dstTarget));
        cmd::named_framebuffer_read_buffer(
            source.m_handle,
            convert::to<group::color_buffer>(srcAttachment, srci));
        cmd::copy_texture_sub_image_2d(
            dstTexture,
            level,
            dstCoord.toVector<i32>(),
            srcRect.bottomleft().toVector<i32>(),
            srcRect.size().convert<i32>());
    } else
#endif
    {
        cmd::bind_framebuffer(
            group::framebuffer_target::framebuffer, dest.m_handle);
        cmd::get_framebuffer_attachment_parameter(
            group::framebuffer_target::framebuffer,
            convert::to(dstAttachment, dsti),
            param::framebuffer_attachment_object_name,
            SpanOne(dstTexture));
        cmd::get_framebuffer_attachment_parameter(
            group::framebuffer_target::framebuffer,
            convert::to(dstAttachment, dsti),
            param::framebuffer_attachment_texture_cube_map_face,
            SpanOne(dstTarget));
        cmd::bind_framebuffer(
            group::framebuffer_target::framebuffer, source.m_handle);
#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x300
        if(features.readdraw_buffers)
            cmd::read_buffer(
                convert::to<group::read_buffer_mode>(srcAttachment, srci));
#endif
        group::texture_target texTarget
            = dstTarget == 0 ? group::texture_target::texture_2d
                             : static_cast<group::texture_target>(dstTarget);
        cmd::bind_texture(texTarget, dstTexture);
        cmd::copy_tex_sub_image_2d(
            texTarget,
            level,
            dstCoord.toVector<i32>(),
            srcRect.bottomleft().toVector<i32>(),
            srcRect.size().convert<i32>());
    }
}

} // namespace gleam
