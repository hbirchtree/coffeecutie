#pragma once

#include <peripherals/base.h>
#include <peripherals/semantic/chunk.h>
#include <platforms/base/file.h>

namespace platform {
namespace file {
namespace embed {

#if __cplusplus__ >= 201703L
using file_reference_t = std::string_view;
#else
using file_reference_t = const char*;
#endif

struct data_descriptor_t
{
    file_reference_t name;
    size_t           offset, size;
};

#if !defined(COFFEE_LINUX)
constexpr bool embeds_enabled = false;
#else
extern bool embeds_enabled;
#endif

extern bool file_lookup(file_reference_t query, semantic::Bytes& data);

} // namespace embed
} // namespace file
} // namespace platform
