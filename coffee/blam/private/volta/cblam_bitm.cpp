#include <coffee/blam/volta/cblam_bitm.h>

#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/CImage>
#include <coffee/core/CMath>
#include <coffee/blam/volta/cblam_bitm_decoders.h>

namespace Coffee{
namespace CBlam{

bitm_header_t _bitm_get_header(const void* base, int32 offset)
{
    const bitm_header_t* ptr = (const bitm_header_t*)(((const byte_t*)base)+offset);
    bitm_header_t hdr;
    CMemCpy(&hdr,ptr,sizeof(bitm_header_t));
    return hdr;
}

const bitm_image_t *coffee_bitm_get(
        const index_item_t *item,
        const file_header_t *map,
        int32 magic,
        int32* numImages)
{
    bitm_header_t hdr = _bitm_get_header(map,item->offset-magic);

    hdr.offset_first -= magic;
    hdr.imageOffset -= magic;

    const bitm_image_t* img = (const bitm_image_t*)(((const byte_t*)map)+hdr.imageOffset);

    return img;
}

bitm_texture_t coffee_bitm_get_texture(const bitm_image_t *img, const void *bitmfile)
{
    CASSERT(img->format <= bitm_format::P8);

    bitm_texture_t def;

    def.mipmaps = CMath::max<int16>(1,img->mipmaps);
    switch(img->type)
    {
    case bitm_type_t::T2D:
        def.type = CTexType::Tex2D;
        break;
    case bitm_type_t::T3D:
        def.type = CTexType::Tex3D;
        break;
    case bitm_type_t::TCube:
        def.type = CTexType::CubeMap;
        break;
    }
    def.resolution.width = img->isize.w;
    def.resolution.height = img->isize.h;
    def.resolution.depth = img->depth;
    def.data = ((ubyte_t*)bitmfile)+img->offset;

    def.cformat = CTexIntFormat::None;

    switch(img->format)
    {
    case bitm_format::P8:
    case bitm_format::A8:
    case bitm_format::Y8:
    case bitm_format::AY8:
        def.format = CTexFormat::RED;
        def.dformat = CDataType::UByte;
        def.cformat = CTexIntFormat::RED8;
        break;
    case bitm_format::A8Y8:
        def.format = CTexFormat::RG;
        def.dformat = CDataType::UByte;
        def.cformat = CTexIntFormat::RG8;
        break;
    case bitm_format::R5G6B5:
        def.format = CTexFormat::RGB;
        def.dformat = CDataType::UShort5_6_5;
        def.cformat = CTexIntFormat::RGB8;
        break;
    case bitm_format::A1R5G5B5:
        def.format = CTexFormat::RGBA;
        def.dformat = CDataType::UShort1_5_5_5_Rev;
        def.cformat = CTexIntFormat::RGBA8;
        break;
    case bitm_format::A4R4G4B4:
        def.format = CTexFormat::RGBA;
        def.dformat = CDataType::UShort4_4_4_4_Rev;
        def.cformat = CTexIntFormat::RGBA8;
        break;
    case bitm_format::A8R8G8B8:
    case bitm_format::X8R8G8B8:
        def.format = CTexFormat::RGBA;
        def.dformat = CDataType::UInt8_8_8_8_Rev;
        def.cformat = CTexIntFormat::RGBA8;
        break;
    case bitm_format::DXT1:
        def.format = CTexFormat::None;
        def.dformat = CDataType::None;
        def.cformat = CTexIntFormat::DXT1;
        break;
    case bitm_format::DXT2AND3:
        def.format = CTexFormat::None;
        def.dformat = CDataType::None;
        def.cformat = CTexIntFormat::DXT3;
        break;
    case bitm_format::DXT4AND5:
        def.format = CTexFormat::None;
        def.dformat = CDataType::None;
        def.cformat = CTexIntFormat::DXT5;
        break;
    };

    return def;
}

}
}
