#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/semantic/chunk.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace b64 {

namespace detail {

/* Defined in private/string_encoding.cpp, so cppcodec stays out of this
 * header. On platforms without it the impl returns {}, as before. */
std::string encode_bytes(void const* data, std::size_t size);

} // namespace detail

/* Reference:
 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 */
template<typename T>
FORCEDINLINE std::string encode(semantic::Span<T> const& data_)
{
    return detail::encode_bytes(data_.data(), data_.size_bytes());
}

std::vector<std::uint8_t> decode(std::string const& data_);

} // namespace b64
