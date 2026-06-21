#pragma once

#include <peripherals/identify/compiler/function_inlining.h>

#include <peripherals/semantic/chunk.h>
#include <string>

#if __has_include(<cppcodec/base64_default_rfc4648.hpp>)
#include <cppcodec/base64_default_rfc4648.hpp>
#define COFFEE_HAS_CPPCODEC 1
#endif

namespace b64 {

/* Reference:
 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 */
template<typename T>
FORCEDINLINE std::string encode(semantic::Span<T> const& data_)
{
#if defined(COFFEE_HAS_CPPCODEC)
    return base64::encode(
        reinterpret_cast<const char*>(data_.data()), data_.size_bytes());
#else
    (void)data_;
    return {};
#endif
}

} // namespace b64
