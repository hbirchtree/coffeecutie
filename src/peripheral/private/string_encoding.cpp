/* Implementations for the string encoders whose headers would otherwise drag
 * <fmt/format.h> and cppcodec into every TU that touches them. */

#include <peripherals/stl/base64.h>
#include <peripherals/stl/string/hexdump.h>
#include <peripherals/stl/string/trim.h>

#include <fmt/format.h>

#if __has_include(<cppcodec/base64_default_rfc4648.hpp>)
#include <cppcodec/base64_default_rfc4648.hpp>
#define COFFEE_HAS_CPPCODEC 1
#endif

namespace stl_types::str::fmt {

std::string hexify(libc_types::u64 s)
{
    std::string str = ::fmt::format("{:016x}", s);
    stl_types::str::trim::left_zero(str);
    return str;
}

std::string hexdump(
    semantic::mem_chunk<const char> const& data,
    bool                                   spacing,
    std::size_t                            newline_freq)
{
    std::string out;
    out.reserve(data.size * 2 /* Hexadec */ + data.size * spacing /* Space */);

    for(std::size_t i = 0; i < data.size; i++)
    {
        out.append(::fmt::format("{:02x}", data[i]));
        if(newline_freq == 0 || (i + 1) % newline_freq != 0)
        {
            if(spacing)
                out.append(" ");
        } else
            out.append("\n");
    }

    return out;
}

} // namespace stl_types::str::fmt

namespace hex {

std::string encode(std::string const& from)
{
    return stl_types::str::fmt::hexdump(
        semantic::mem_chunk<const char>::ofContainer(from), false);
}

} // namespace hex

namespace b64 {

namespace detail {

std::string encode_bytes(void const* data, std::size_t size)
{
#if defined(COFFEE_HAS_CPPCODEC)
    return base64::encode(reinterpret_cast<const char*>(data), size);
#else
    (void)data;
    (void)size;
    return {};
#endif
}

} // namespace detail

std::vector<std::uint8_t> decode(std::string const& data_)
{
#if defined(COFFEE_HAS_CPPCODEC)
    return base64::decode(data_);
#else
    (void)data_;
    return {};
#endif
}

} // namespace b64
