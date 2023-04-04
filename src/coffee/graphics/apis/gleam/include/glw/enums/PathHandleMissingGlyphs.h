#pragma once

#include "common.h"

namespace gl::group {

// PathHandleMissingGlyphs
enum class path_handle_missing_glyphs : u32
{
#ifdef GL_SKIP_MISSING_GLYPH_NV
    skip_missing_glyph_nv = GL_SKIP_MISSING_GLYPH_NV,
#endif
#ifdef GL_USE_MISSING_GLYPH_NV
    use_missing_glyph_nv = GL_USE_MISSING_GLYPH_NV,
#endif
}; // enum class path_handle_missing_glyphs

} // namespace gl::group
