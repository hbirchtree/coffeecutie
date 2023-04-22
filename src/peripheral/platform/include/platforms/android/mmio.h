#pragma once

#include "../posix/mmio.h"
#include "detail.h"

namespace platform::file::android {
namespace detail {

struct android_mapping_t
{
    posix::mem_mapping_t mapping;
    AAsset*              asset{nullptr};

    operator posix::mem_mapping_t::span_type()
    {
        return mapping.view;
    }
};

} // namespace detail

FORCEDINLINE result<detail::android_mapping_t, posix::posix_error> map(
    Url const& file, posix::mapping_params_t params)
{
    while(detail::is_asset(file, params.access))
    {
        auto asset = detail::open_asset(file);
        /* If we can't find an asset by this name, try searching for something
         * on the filesystem */
        if(!asset)
            break;
        auto const_mapping
            = semantic::mem_chunk<const char>::ofBytes(
                  AAsset_getBuffer(asset), AAsset_getLength64(asset))
                  .view;
        auto unsafe_mapping = semantic::mem_chunk<char>::of(
            const_cast<char*>(const_mapping.data()), const_mapping.size());
        return detail::android_mapping_t{
            .mapping = {
                .view = unsafe_mapping.view,
                .access = RSCA::ReadOnly,
            },
            .asset = asset,
        };
    }

    if(auto res = posix::map(file, params); res.has_value())
    {
        return detail::android_mapping_t{
            .mapping = std::move(res.value()),
        };
    } else
        return res.error();
}

FORCEDINLINE std::optional<posix::posix_error> unmap(
    detail::android_mapping_t&& mapping)
{
    if(mapping.asset)
    {
        AAsset_close(mapping.asset);
        return std::nullopt;
    } else
        return posix::unmap(std::move(mapping.mapping));
}

} // namespace platform::file::android
