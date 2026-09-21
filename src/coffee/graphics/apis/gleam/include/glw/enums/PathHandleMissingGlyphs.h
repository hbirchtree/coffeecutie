#pragma once

#include "common.h"

namespace gl::group {

// PathHandleMissingGlyphs
enum class path_handle_missing_glyphs : u32
{
    skip_missing_glyph_nv = 0x90A9, // GL_SKIP_MISSING_GLYPH_NV
    use_missing_glyph_nv  = 0x90AA, // GL_USE_MISSING_GLYPH_NV
}; // enum class path_handle_missing_glyphs

} // namespace gl::group
