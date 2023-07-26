#pragma once

#include <peripherals/identify/compiler/function_inlining.h>

#include <cppcodec/base64_default_rfc4648.hpp>
#include <peripherals/semantic/chunk.h>
#include <string>

namespace b64 {

/* Reference:
 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 */
template<typename T>
FORCEDINLINE std::string encode(semantic::Span<T> const& data_)
{
    return base64::encode(
        reinterpret_cast<const char*>(data_.data()), data_.size_bytes());
}

} // namespace b64
