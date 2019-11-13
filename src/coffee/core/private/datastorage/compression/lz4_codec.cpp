#include <coffee/compression/lz4.h>

#include <coffee/core/base.h>
#include <lz4hc.h>

namespace lz4 {

bool compressor::Compress(
    semantic::Bytes const&  uncompressed,
    semantic::Bytes*        target,
    compressor::opts const& opts,
    compressor::error_code& ec)
{


    return false;
}

bool compressor::Decompress(
    semantic::Bytes const&  compressed,
    semantic::Bytes*        target,
    compressor::opts const& opts,
    compressor::error_code& ec)
{
    return false;
}

} // namespace lz4
