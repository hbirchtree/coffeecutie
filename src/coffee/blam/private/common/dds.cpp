#include <blam/common/dds.h>

namespace blam::dds {

namespace {

void append(std::vector<char>& out, u32 value)
{
    auto const* bytes = reinterpret_cast<char const*>(&value);
    out.insert(out.end(), bytes, bytes + sizeof(value));
}

} // namespace

std::vector<char> make(info_t const& info, u32 width, u32 height, semantic::Span<const u8> pixels, bool cube_map)
{
    std::vector<char> out;
    out.reserve(128 + pixels.size());
    append(out, 0x20534444); /* 'DDS ' */
    append(out, 124);
    append(out, 0x1007); /* CAPS|HEIGHT|WIDTH|PIXELFORMAT */
    append(out, height);
    append(out, width);
    append(out, static_cast<u32>(pixels.size()) / (cube_map ? 6 : 1));
    /* depth, mipmap count, reserved[11] */
    for(int i = 0; i < 13; i++)
        append(out, 0);
    append(out, 32); /* pixel format size */
    if(info.fourcc)
    {
        append(out, 0x4); /* FOURCC */
        append(out, info.fourcc);
        for(int i = 0; i < 5; i++)
            append(out, 0);
    } else
    {
        append(out, info.pf_flags);
        append(out, 0);
        append(out, info.bpp);
        append(out, info.mask_r);
        append(out, info.mask_g);
        append(out, info.mask_b);
        append(out, info.mask_a);
    }
    /* CAPS_TEXTURE, plus COMPLEX + all cube faces for cube maps */
    append(out, cube_map ? 0x1008 : 0x1000);
    append(out, cube_map ? 0xFE00 : 0);
    for(int i = 0; i < 3; i++)
        append(out, 0);
    out.insert(out.end(), pixels.begin(), pixels.end());
    return out;
}

} // namespace blam::dds
