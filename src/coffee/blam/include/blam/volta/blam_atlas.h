#pragma once

#include "blam_magic_data.h"
#include "blam_reference.h"

namespace blam {

struct locator_block
{
    u32 name_offset;
    u32 size;
    u32 offset;

    template<typename T>
    inline auto to_reference() const
    {
        return reference<T>{1, {offset}};
    }
};

struct tag_atlas_t
{
    atlas_type_t type;
    u32 name_block_off;

    u32 locators_offset;
    u32 locators_count;

    static inline tag_atlas_t const* from_data(
        semantic::Span<const byte_t> const& data)
    {
        return C_RCAST<tag_atlas_t const*>(data.data());
    }

    inline std::string_view get_name(locator_block const& block) const
    {
        auto base_ptr = C_RCAST<byte_t const*>(this);

        return C_RCAST<const char*>(
            base_ptr + name_block_off + block.name_offset);
    }

    inline semantic::Span<locator_block const> locators() const
    {
        auto base_ptr = C_RCAST<byte_t const*>(this);

        return semantic::Span<locator_block const>(
            C_RCAST<locator_block const*>(base_ptr + locators_offset),
            locators_count);
    }

    inline std::optional<locator_block const*> by_name(
        std::string_view name, map_ptr const& magic) const
    {
        for(auto const& loc : locators())
            if(get_name(loc) == name)
                return &loc;
        return std::nullopt;
    }

    inline map_ptr block_magic(map_ptr const& bitm, u32 bitm_idx) const
    {
        auto const& loc = locators()[bitm_idx];
        return map_ptr(
            {bitm.base_ptr + loc.offset, bitm.max_size - loc.offset}, 0);
    }

    template<typename T>
    inline reference<T> get_block(u32 bitm_idx = 0) const
    {
        auto const& loc = locators()[bitm_idx];
        return loc.to_reference<T>();
    }
};

struct atlas_view
{
    map_ptr            magic;
    tag_atlas_t const* header;

    static inline atlas_view from_data(semantic::Span<const byte_t> const& data)
    {
        return {
            .magic  = map_ptr(data),
            .header = tag_atlas_t::from_data(data),
        };
    }

    static inline atlas_view from_ptr(map_ptr const& data)
    {
        return from_data(data.data());
    }
};

} // namespace blam
