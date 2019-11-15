#pragma once

#include "standard.h"

namespace lz4 {

enum class lz4_error
{
    no_error,
    exceeds_file_size_limit,
    compression_failed,

    not_enough_data,
    malformed_header,
    decompression_mismatch_size,
    decompression_failed,
};

struct lz4_error_category : stl_types::error_category
{
    virtual const char* name() const noexcept override;
    virtual std::string message(int) const override;
};

using lz4_error_code =
    stl_types::domain_error_code<lz4_error, lz4_error_category>;

struct compressor : Coffee::Compression::Compressor_def
{
    using error_code = lz4_error_code;

    enum class compression_mode
    {
        default_,
        fast,
        high,
    };

    struct opts
    {
        compression_mode mode;

        int fast_acceleration; /*!< For compression_mode::fast */
        int high_compression;  /*!< For compression_mode::high */
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
