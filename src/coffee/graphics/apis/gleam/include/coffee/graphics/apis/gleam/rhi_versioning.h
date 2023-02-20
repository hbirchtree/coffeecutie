#pragma once

#include <glw/glw.h>
#include <peripherals/concepts/graphics_api.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/handle.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/types.h>

namespace gleam {
namespace group {
using namespace gl::group;
}

template<typename T = std::byte>
struct null_span
{
    using value_type = T;
    using size_type  = size_t;

    template<typename T2>
    static inline null_span of(T2 size)
    {
        return {static_cast<size_t>(size)};
    }

    inline std::byte* data() const
    {
        return nullptr;
    }

    inline size_t size() const
    {
        return m_size;
    }

    inline std::byte const& operator[](size_t) const
    {
        return m_zero;
    }

    size_t    m_size{0};
    std::byte m_zero{0};
};

struct offset_span
{
    using value_type = std::byte;
    using size_type  = size_t;

    template<typename T2>
    static inline offset_span of(T2 offset)
    {
        return {static_cast<size_t>(offset)};
    }

    inline void* data() const
    {
        return reinterpret_cast<void*>(m_offset);
    }
    inline size_t size() const
    {
        return 1;
    }
    inline std::byte const& operator[](size_t) const
    {
        return m_zero;
    }

    size_t    m_offset{0};
    std::byte m_zero{0};
};

using namespace semantic::concepts::graphics;
using semantic::mem_chunk;
using semantic::Span;
using semantic::SpanOne;
using stl_types::MkShared;
using stl_types::ShPtr;
using typing::geometry::size_2d;
using typing::geometry::size_3d;
using typing::pixels::PixDesc;
using typing::vector_types::Veci2;
using typing::vector_types::Veci3;
using typing::vector_types::Vecui2;
using typing::vector_types::Vecui3;

using cmd =
#if GLEAM_MAX_VERSION == 0x330
    gl::core::v33
#elif GLEAM_MAX_VERSION == 0x400
    gl::core::v40
#elif GLEAM_MAX_VERSION == 0x410
    gl::core::v41
#elif GLEAM_MAX_VERSION == 0x420
    gl::core::v42
#elif GLEAM_MAX_VERSION == 0x430
    gl::core::v43
#elif GLEAM_MAX_VERSION == 0x440
    gl::core::v44
#elif GLEAM_MAX_VERSION == 0x450
    gl::core::v45
#elif GLEAM_MAX_VERSION >= 0x460
    gl::impl::core<gl::core::highest>
#elif GLEAM_MAX_VERSION_ES == 0x200
    gl::es::v20
#elif GLEAM_MAX_VERSION_ES == 0x300
    gl::es::v30
#elif GLEAM_MAX_VERSION_ES == 0x310
    gl::es::v31
#elif GLEAM_MAX_VERSION_ES == 0x320
    gl::impl::es<gl::es::highest>
#endif
    ;

using lowest_version =
#ifdef GL_VERSION_1_0
    gl::core::lowest
#elif defined(GL_ES_VERSION_2_0)
    gl::es::lowest
#endif
    ;

enum class api_type_t
{
    none,
    core,
    es,
};
constexpr api_type_t compiled_api =
#if GLEAM_MAX_VERSION != 0
    api_type_t::core;
#else
    api_type_t::es;
#endif

constexpr auto platform_version = cmd::version{};

using hnd = semantic::
    generic_handle_t<libc_types::u32, semantic::handle_modes::no_op>;

} // namespace gleam
