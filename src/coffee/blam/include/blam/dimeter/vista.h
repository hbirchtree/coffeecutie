#pragma once

#if __has_include(<corez/zlib.h>)
#include <corez/zlib.h>
#define DIMETER_HAS_ZLIB 1
#endif

#include <optional>
#include <vector>

#include "structures.h"

/*! What only the Vista maps need: their bitmap lods and lightmaps ship
 * as zlib streams where the Xbox stores raw pixels. */
namespace blam::dimeter::vista {

/*! Vista keeps every bitmap lod as a zlib stream (Xbox stores them raw):
 * CMF 0x78 and a check-divisible FLG */
inline bool is_zlib(Span<const u8> bytes)
{
    if(bytes.size() < 2)
        return false;
    u8 cmf = static_cast<u8>(bytes[0]), flg = static_cast<u8>(bytes[1]);
    return cmf == 0x78 && ((cmf << 8) | flg) % 31 == 0;
}

/*! Inflates a lod; nullopt without zlib or on a broken stream */
inline std::optional<std::vector<u8>> inflate(Span<const u8> bytes)
{
#if defined(DIMETER_HAS_ZLIB)
    std::vector<char> out;
    auto              err = ::zlib::codec::decompress(
        Span<const u8>(reinterpret_cast<u8 const*>(bytes.data()), bytes.size()),
        out,
        ::zlib::options_t{.chunk_size = 1u << 20});
    if(err)
        return std::nullopt;
    return std::vector<u8>(out.begin(), out.end());
#else
    (void)bytes;
    return std::nullopt;
#endif
}

} // namespace blam::dimeter::vista
