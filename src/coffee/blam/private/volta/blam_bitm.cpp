#include <blam/volta/blam_bitm.h>

#include <blam/volta/blam_atlas.h>

#include <glw/texture_formats.h>
#include <glw/texture_formats_desc.h>
#include <peripherals/stl/range.h>

namespace blam::bitm {

std::tuple<typing::pixels::bit_fmt, typing::pix_components> image_t::to_fmt()
    const
{
    switch(format)
    {
    case format_t::A8:
    case format_t::Y8:
    case format_t::P8:
    case format_t::AY8:
        return {bit_fmt::u8, pix_components::R};

    case format_t::A8Y8:
        return {bit_fmt::u8, pix_components::RG};
    case format_t::R5G6B5:
        return {bit_fmt::u16_565, pix_components::RGB};
    case format_t::A1RGB5:
        return {bit_fmt::u16_1555, pix_components::RGBA};
    case format_t::ARGB4:
        return {bit_fmt::u16_4444, pix_components::RGBA};

    case format_t::ARGB8:
        return {bit_fmt::u8, pix_components::RGBA};
    case format_t::XRGB8:
        return {bit_fmt::u8, pix_components::RGBA};

    default:
        return {bit_fmt::undefined, pix_components::None};
    }
}

typing::pixels::pix_fmt image_t::to_pixfmt() const
{
    switch(format)
    {
    case format_t::A8:
    case format_t::Y8:
    case format_t::P8:
    case format_t::AY8:
        return pix_fmt::R8;
    case format_t::R5G6B5:
        return pix_fmt::RGB565;
    case format_t::A8Y8:
        return pix_fmt::RG8;
    case format_t::A1RGB5:
        return pix_fmt::RGB5A1;
    case format_t::ARGB4:
        return pix_fmt::RGBA4;
    case format_t::ARGB8:
        return pix_fmt::RGBA8;
    case format_t::XRGB8:
        return pix_fmt::RGBA8;
    case format_t::BC1:
    case format_t::BC2:
    case format_t::BC3:
        return pix_fmt::BCn;
    }
    throw std::runtime_error("unhandled pixfmt");
}

std::tuple<typing::pixels::pix_fmt, typing::pixels::comp_flags> image_t::
    to_compressed_fmt() const
{
    switch(format)
    {
    case format_t::BC1:
        return {pix_fmt::BCn, comp_flags::BC1};
    case format_t::BC2:
        return {pix_fmt::BCn, comp_flags::BC2};
    case format_t::BC3:
        return {pix_fmt::BCn, comp_flags::BC3};
    default:
        break;
    }
    throw std::runtime_error("unhandled compressed fmt");
}

semantic::Span<const libc_types::u8> image_t::data(
    const map_ptr& magic, u16 mipmap) const
{
    using namespace typing::pixels::properties;

    if(mipmap != 0 && mipmap >= mipmaps)
        Throw(undefined_behavior("mipmap out of range"));

    auto mipsize = isize;
    mipsize.x >>= mipmap;
    mipsize.y >>= mipmap;

    if(!compressed())
    {
        auto const& format =
            gl::tex::format_of(typing::pixels::PixDesc(to_pixfmt()));

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

        return reference<u8>{.count = size, .offset = offset + mip_offset}
            .data(magic)
            .value();
    } else
    {
        pix_fmt    fmt;
        comp_flags flags;
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

        return reference<u8>{.count = size, .offset = offset + mip_offset}
            .data(magic)
            .value();
    }
}

} // namespace blam::bitm

namespace blam {

stl_types::result<std::pair<const bitm::header_t*, map_ptr>, error_msg> tag_t::
    image(const map_ptr& magic, const atlas_view& source) const
{
    /* CE changes! */
    if(storage == tag_storage_t::external)
    {
        return std::make_pair(
            source.header->get_block<bitm::header_t>(offset)
                .data(source.magic)
                .value()
                .data(),
            source.header->block_magic(source.magic, offset));
    } else
    {
        return std::make_pair(data<bitm::header_t>(magic).value(), magic);
    }
}

} // namespace blam
