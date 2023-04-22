#pragma once

#include <glw/texture_format.h>
#include <peripherals/error/result.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/vector_types.h>

struct ktxTexture;

struct ktxDeleter
{
    void operator()(ktxTexture* ptr);
};

namespace ktx {

using libc_types::u32;
using typing::vector_types::Vecui2;

struct texture_t
{
    gl::tex::texture_format_t format{};
    u32                       levels{};
    struct mipmap_t
    {
        semantic::Span<const char>   data;
        typing::vector_types::Vecui3 size;
    };
    std::vector<mipmap_t> mips;

    std::unique_ptr<ktxTexture, ktxDeleter> texture;
};

stl_types::result<texture_t, std::string> load_from(
    semantic::Span<const char>&& data);

} // namespace ktx
