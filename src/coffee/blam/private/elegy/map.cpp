#include <blam/elegy/map.h>

namespace blam::elegy {

std::string_view map_container::table_string(
    u32 position,
    u32 count,
    u32 index_address,
    u32 buffer_address,
    u32 buffer_size) const
{
    u32 map_size     = static_cast<u32>(data.size());
    u32 index_offset = header->section_offset(section_t::debug, index_address);
    u32 buffer_offset =
        header->section_offset(section_t::debug, buffer_address);
    if(position >= count || index_offset >= map_size ||
       (map_size - index_offset) / 4 <= position ||
       buffer_offset >= map_size || buffer_size > map_size - buffer_offset)
        return {};
    u32 offset;
    std::memcpy(&offset, data.data() + index_offset + position * 4, 4);
    offset = from_be(offset);
    if(offset >= buffer_size)
        return {};
    auto const* start =
        reinterpret_cast<char const*>(data.data() + buffer_offset + offset);
    auto const* end = static_cast<char const*>(
        std::memchr(start, 0, buffer_size - offset));
    return std::string_view(start, end ? end - start : buffer_size - offset);
}

} // namespace blam::elegy
