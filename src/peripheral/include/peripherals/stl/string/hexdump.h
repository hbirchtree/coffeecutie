#pragma once

#include "hex.h"

#include <peripherals/semantic/chunk.h>

#include <cstddef>
#include <string>

/* Split out of hex.h: semantic::mem_chunk drags in chunk.h, which most users
 * of pointerify() have no need for. */

namespace stl_types::str::fmt {

std::string hexdump(
    semantic::mem_chunk<const char> const& data,
    bool                                   spacing      = true,
    std::size_t                            newline_freq = 0);

} // namespace stl_types::str::fmt

namespace hex {

std::string encode(std::string const& from);

} // namespace hex
