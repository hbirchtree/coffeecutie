#pragma once

#include "standard.h"

namespace lz4 {

enum class error
{
    no_error,
    exceeds_file_size_limit,
    compression_failed,

    not_enough_data,
    malformed_header,
    decompression_mismatch_size,
    decompression_failed,
};

struct error_category : stl_types::error_category
{
    virtual const char* name() const noexcept override;
    virtual std::string message(int) const override;
};

using error_code = stl_types::domain_error_code<error, error_category>;

struct compressor : Coffee::Compression::Compressor_def
{
    enum class compression_mode
    {
        default_,
        fast,
        high,
    };

    struct opts
    {
        STATICINLINE opts high_comp(int level)
        {
            return {compression_mode::high, 0, level};
        }
        STATICINLINE opts fast_comp(int accel)
        {
            return {compression_mode::fast, accel, 0};
        }

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
