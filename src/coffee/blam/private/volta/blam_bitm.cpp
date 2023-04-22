#include <blam/volta/blam_bitm.h>

#include <glw/texture_formats.h>

namespace blam::bitm {

std::tuple<typing::pixels::BitFmt, typing::PixCmp> image_t::to_fmt() const
{
    switch(format)
    {
    case format_t::A8:
    case format_t::Y8:
    case format_t::P8:
    case format_t::AY8:
        return {BitFmt::UByte, PixCmp::R};

    case format_t::A8Y8:
        return {BitFmt::UByte, PixCmp::RG};
    case format_t::R5G6B5:
        return {BitFmt::UShort_565, PixCmp::RGB};
    case format_t::A1RGB5:
        return {BitFmt::UShort_1555R, PixCmp::RGBA};
    case format_t::ARGB4:
        return {BitFmt::UShort_4444, PixCmp::RGBA};

    case format_t::ARGB8:
        return {BitFmt::UByte, PixCmp::RGBA};
    case format_t::XRGB8:
        return {BitFmt::UByte, PixCmp::RGBA};

    default:
        return {BitFmt::Undefined, PixCmp::None};
    }
}

typing::pixels::PixFmt image_t::to_pixfmt() const
{
    switch(format)
    {
    case format_t::A8:
    case format_t::Y8:
    case format_t::P8:
    case format_t::AY8:
        return PixFmt::R8;
    case format_t::R5G6B5:
        return PixFmt::RGB565;
    case format_t::A8Y8:
        return PixFmt::RG8;
    case format_t::A1RGB5:
        return PixFmt::RGB5A1;
    case format_t::ARGB4:
        return PixFmt::RGBA4;
    case format_t::ARGB8:
        return PixFmt::RGBA8;
    case format_t::XRGB8:
        return PixFmt::RGBA8;
    case format_t::BC1:
    case format_t::BC2:
    case format_t::BC3:
        return PixFmt::BCn;
    }
    throw std::runtime_error("unhandled pixfmt");
}

std::tuple<typing::pixels::PixFmt, typing::pixels::CompFlags> image_t::
    to_compressed_fmt() const
{
    switch(format)
    {
    case format_t::BC1:
        return {PixFmt::BCn, CompFlags::BC1};
    case format_t::BC2:
        return {PixFmt::BCn, CompFlags::BC2};
    case format_t::BC3:
        return {PixFmt::BCn, CompFlags::BC3};
    default:
        break;
    }
    throw std::runtime_error("unhandled compressed fmt");
}

semantic::Span<const libc_types::u8> image_t::data(
    const magic_data_t& magic, u16 mipmap) const
{
    using namespace typing::pixels::properties;

    if(mipmap != 0 && mipmap >= mipmaps)
        Throw(undefined_behavior("mipmap out of range"));

    auto mipsize = isize;
    mipsize.x >>= mipmap;
    mipsize.y >>= mipmap;

    if(!compressed())
    {
        auto const& format
            = gl::tex::format_of(typing::pixels::PixDesc(to_pixfmt()));

        u32 size       = format.data_size(mipsize);
        u32 mip_offset = 0;

        for(auto i : stl_types::Range<>(mipmap))
        {
            auto imsize = isize;
            imsize.x >>= i;
            imsize.y >>= i;
            mip_offset += format.data_size(imsize);
        }

        if(type == type_t::tex_cube)
            size *= 6;
        else if(type == type_t::tex_3d)
            size *= depth;

        return reflexive_t<u8>{.count = size, .offset = offset + mip_offset}
            .data(magic)
            .value();
    } else
    {
        PixFmt    fmt;
        CompFlags flags;
        std::tie(fmt, flags) = to_compressed_fmt();
        auto comp_fmt        = typing::pixels::CompFmt(fmt, flags);

        u32 size = gl::tex::format_of(comp_fmt).data_size(mipsize);

        u32 mip_offset = 0;
        for(auto i : stl_types::Range<>(mipmap))
        {
            auto off_size = isize;
            off_size.x >>= i;
            off_size.y >>= i;
            mip_offset += gl::tex::format_of(comp_fmt).data_size(off_size);
        }

        if(type == type_t::tex_cube)
            size *= 6;
        else if(type == type_t::tex_3d)
            size *= depth;

        return reflexive_t<u8>{.count = size, .offset = offset + mip_offset}
            .data(magic)
            .value();
    }
}

} // namespace blam::bitm

namespace blam {

stl_types::result<std::pair<const bitm::header_t*, magic_data_t>, error_msg>
tag_t::image(
    const magic_data_t& magic, const bitm::bitmap_atlas_view& source) const
{
    /* CE changes! */
    if(storage == image_storage_t::external)
    {
        return std::make_pair(
            source.header->get_block(offset)
                .data(source.bitmap_magic)
                .value()
                .data(),
            source.header->block_magic(source.bitmap_magic, offset));
    } else
    {
        return std::make_pair(data<bitm::header_t>(magic).value(), magic);
    }
}

} // namespace blam
