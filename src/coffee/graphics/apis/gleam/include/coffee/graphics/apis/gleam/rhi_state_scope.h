#pragma once

#include "rhi_versioning.h"

namespace gleam::state {

struct framebuffer_scope_t
{
    framebuffer_scope_t(group::framebuffer_target bind_target, u32 handle)
        : m_target(bind_target)
    {
        auto bind_target_get = [bind_target] {
            switch(bind_target)
            {
            case group::framebuffer_target::read_framebuffer:
                return group::get_prop::read_framebuffer_binding;
            case group::framebuffer_target::draw_framebuffer:
                return group::get_prop::draw_framebuffer_binding;
            default:
                return group::get_prop::framebuffer_binding;
            }
        }();
        cmd::get_integerv(bind_target_get, semantic::SpanOne(m_previous));
        cmd::bind_framebuffer(bind_target, handle);
    }
    ~framebuffer_scope_t()
    {
        cmd::bind_framebuffer(m_target, m_previous);
    }
    group::framebuffer_target m_target{};
    i32                       m_previous{};
};

struct viewport_scope_t
{
    viewport_scope_t(typing::vector_types::Veci4 const& new_view)
    {
        cmd::get_integerv(
            group::get_prop::viewport,
            semantic::Span<i32>(&m_previous.x, 4));
        cmd::viewport(
            typing::vector_types::Veci2{new_view.x, new_view.y},
            typing::vector_types::Veci2{new_view.z, new_view.w});
    }
    ~viewport_scope_t()
    {
        cmd::viewport(
            typing::vector_types::Veci2{m_previous.x, m_previous.y},
            typing::vector_types::Veci2{m_previous.z, m_previous.w});
    }
    typing::vector_types::Veci4 m_previous{};
};

}
