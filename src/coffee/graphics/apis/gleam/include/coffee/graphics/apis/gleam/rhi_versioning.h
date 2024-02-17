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

using semantic::concepts::null_span;
using semantic::concepts::offset_span;

using namespace semantic::concepts::graphics;
using semantic::mem_chunk;
using semantic::Span;
using semantic::SpanOne;
using std::make_shared;
using std::shared_ptr;
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
    webgl,
};
constexpr api_type_t compiled_api =
#if GLEAM_MAX_VERSION != 0
    api_type_t::core;
#else
    api_type_t::es;
#endif
constexpr api_type_t platform_api =
#if defined(COFFEE_EMSCRIPTEN)
    api_type_t::webgl;
#elif GLEAM_MAX_VERSION != 0
    api_type_t::core;
#else
        api_type_t::es;
#endif

constexpr auto platform_version = cmd::version{};

using hnd =
    semantic::generic_handle_t<libc_types::u32, semantic::handle_modes::no_op>;

} // namespace gleam
