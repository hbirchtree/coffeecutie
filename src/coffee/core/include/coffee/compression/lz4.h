#pragma once

#include "standard.h"

namespace lz4 {

using lz4_error_code =
    stl_types::domain_error_code<int, stl_types::error_category>;

struct compressor : Coffee::Compression::Compressor_def
{
    using error_code = lz4_error_code;

    struct limits
    {
        static constexpr libc_types::u32 min_compress = 1;
        static constexpr libc_types::u32 opt_compress = 9;
        static constexpr libc_types::u32 max_compress = 9;
    };

    struct opts
    {
        libc_types::u32 level = limits::opt_compress;
    };

    static bool Compress(
        semantic::Bytes const& uncompressed,
        semantic::Bytes*       target,
        opts const&            opts,
        error_code&            ec);

    static bool Decompress(
            semantic::Bytes const& compressed,
            semantic::Bytes*       target,
            opts const&            opts,
            error_code&            ec);
};

} // namespace lz4
