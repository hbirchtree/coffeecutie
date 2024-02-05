#pragma once

#include "rhi_features.h"

#include <coffee/core/files/cfiles.h>
#include <fmt/format.h>

namespace gleam::literals {
namespace detail {

struct TextureUrl
{
    const char* path{nullptr};

    Coffee::Resource with(
        features const& features,
        semantic::RSCA  storage = semantic::RSCA::AssetFile) const;
};

} // namespace detail

inline detail::TextureUrl operator"" _texture(const char* url, size_t)
{
    return {.path = url};
}

} // namespace gleam::literals
