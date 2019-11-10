#pragma once

#include <coffee/core/libc_types.h>

namespace Coffee {
namespace Display {

/*!
 * \brief Bit depths for context
 */
struct ContextBits
{
    ContextBits() : red(0), green(0), blue(0), alpha(0), depth(0), stencil(0)
    {
    }

    bool operator==(ContextBits const& other)
    {
        return (
            red == other.red && green == other.green && blue == other.blue &&
            alpha == other.alpha && depth == other.depth &&
            stencil == other.stencil && samples == other.samples);
    }

    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;

    u8 depth;
    u8 stencil;

    u8 samples;

    u8 buffer_size;
};

} // namespace Display
} // namespace Coffee
