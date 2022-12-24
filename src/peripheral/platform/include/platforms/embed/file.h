#pragma once

#include <peripherals/base.h>
#include <peripherals/semantic/chunk.h>
#include <platforms/base/file.h>

namespace platform::file::embed {

using file_reference_t = std::string_view;

struct data_descriptor_t
{
    file_reference_t name;
    size_t           offset, size;
};

#if !defined(COFFEE_LINUX)
constexpr bool embeds_enabled = false;
#else
extern const bool embeds_enabled;
#endif

namespace detail {
extern semantic::Span<const data_descriptor_t> files_listing();
extern semantic::Span<const libc_types::u8>    files_data();
} // namespace detail

FORCEDINLINE std::optional<semantic::Span<const libc_types::u8>> file_lookup(
    file_reference_t query)
{
    auto range = detail::files_listing();
    auto it    = std::find_if(
        range.begin(), range.end(), [query](data_descriptor_t const& e) {
            return e.name == query;
        });

    if(it == range.end())
        return std::nullopt;

    return detail::files_data().subspan(it->offset, it->size);
}

} // namespace platform::file::embed
