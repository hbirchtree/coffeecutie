#include <coffee/blam/volta/cblam_bitm.h>

#include <coffee/blam/volta/cblam_bitm_decoders.h>
#include <coffee/core/CFiles>
#include <coffee/core/CMath>
#include <coffee/core/types/chunk.h>
#include <coffee/image/cimage.h>

#include <coffee/core/CDebug>

namespace blam {

using semantic::Bytes;
using namespace semantic::chunk_ops;

bitm_header_t _bitm_get_header(const void* base, i32 offset)
{
    const bitm_header_t* ptr =
        C_RCAST<const bitm_header_t*>((C_RCAST<const byte_t*>(base)) + offset);
    bitm_header_t hdr = {};
    if(ptr->imageCount < 1)
        return hdr;
    MemCpy(Bytes::Create(*ptr), Bytes::Create(hdr));
    return hdr;
}

const bitm_image_t* bitm_get(
    const index_item_t*  item,
    const file_header_t* map,
    i32                  magic,
    C_UNUSED(i32* numImages))
{
    bitm_header_t hdr = _bitm_get_header(map, item->offset - magic);

    if(!hdr.imageCount)
        return nullptr;

    hdr.offset_first -= magic;
    hdr.imageOffset -= magic;

    const bitm_image_t* img = C_RCAST<const bitm_image_t*>(
        (C_RCAST<const byte_t*>(map)) + hdr.imageOffset);

    return img;
}

bitm_texture_t bitm_get_texture(bitm_image_t const* img, c_cptr bitmfile)
{
    if(img->format > bitm_format::P8)
        Throw(undefined_behavior("unrecognized bitmap format"));

    bitm_texture_t def;

    def.mipmaps = stl_types::math::max<i16>(1, img->mipmaps);
    switch(img->type)
    {
    case bitm_type_t::T2D:
        def.type = TexType::T2D;
        break;
    case bitm_type_t::T3D:
        def.type = TexType::T3D;
        break;
    case bitm_type_t::TCube:
        def.type = TexType::Cube;
        break;
    }
    def.resolution.width  = img->isize.w;
    def.resolution.height = img->isize.h;
    def.resolution.depth  = img->depth;
    def.data              = ((ubyte_t*)bitmfile) + img->offset;

    def.cformat = PixFmt::RGBA8;

    switch(img->format)
    {
    case bitm_format::P8:
    case bitm_format::A8:
    case bitm_format::Y8:
    case bitm_format::AY8:
        def.format  = PixCmp::R;
        def.dformat = BitFmt::UInt;
        def.cformat = PixFmt::R8UI;
        break;
    case bitm_format::A8Y8:
        def.format  = PixCmp::RG;
        def.dformat = BitFmt::UInt;
        def.cformat = PixFmt::RG8UI;
        break;
    case bitm_format::R5G6B5:
        def.format  = PixCmp::BGR;
        def.dformat = BitFmt::UShort_565;
        def.cformat = PixFmt::RGB8UI;
        break;
    case bitm_format::A1R5G5B5:
        def.format  = PixCmp::BGRA;
        def.dformat = BitFmt::UShort_1555R;
        def.cformat = PixFmt::RGBA8;
        break;
    case bitm_format::A4R4G4B4:
        def.format  = PixCmp::BGRA;
        def.dformat = BitFmt::UShortR;
        def.cformat = PixFmt::RGBA8;
        break;
    case bitm_format::A8R8G8B8:
    case bitm_format::X8R8G8B8:
        def.format  = PixCmp::BGRA;
        def.dformat = BitFmt::UIntR;
        def.cformat = PixFmt::RGBA8;
        break;
    case bitm_format::DXT1:
        def.format  = PixCmp::RGB;
        def.dformat = BitFmt::Byte;
        def.cformat = PixFmt::S3TC;
        break;
    case bitm_format::DXT2AND3:
        def.format  = PixCmp::RGBA;
        def.dformat = BitFmt::Byte;
        def.cformat = PixFmt::S3TC;
        break;
    case bitm_format::DXT4AND5:
        def.format  = PixCmp::RGBA;
        def.dformat = BitFmt::Byte;
        def.cformat = PixFmt::S3TC;
        break;
    };

    return def;
}

} // namespace blam
