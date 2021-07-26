#pragma once

#include <concepts>
#include <peripherals/semantic/chunk.h>
#include <url/url.h>

namespace semantic::concepts {
namespace detail {

using byte_chunk_t = mem_chunk<libc_types::byte_t>;
using const_byte_chunk_t = mem_chunk<libc_types::byte_t>;

inline void casts_to_bytes(byte_chunk_t const&)
{
}
inline void casts_to_const_bytes(const_byte_chunk_t const&)
{
}
inline void casts_to_path(platform::url::Path const&)
{
}

} // namespace detail

template<typename T>
concept Resource = requires(T rsc)
{
    { detail::casts_to_bytes(rsc) };
    { detail::casts_to_const_bytes(rsc) };
    { detail::casts_to_path(rsc) };
    { rsc = detail::byte_chunk_t() };
    { rsc = detail::const_byte_chunk_t() };
    { *rsc };
    std::is_same_v<decltype(rsc.valid()), bool>;
};

} // namespace semantic::concepts
