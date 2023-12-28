#include "encoder.h"

#include <glad/gl.h>
#include <ktx.h>
#include <magic_enum.hpp>
#include <fmt/format.h>

#define RGBCX_IMPLEMENTATION 1
#include <bc7enc/bc7enc.h>
#include <bc7enc/rgbcx.h>

namespace compressor::etc2 {

std::optional<ktxTexture1*> encode(
    const rgbaf_image_t& img, format_t format, settings_t&& settings)
{
    std::vector<Etc::RawImage> images;
    images.resize(settings.mipmaps);

    int encoding_time = 0;

    Etc::EncodeMipmaps(
        img.data,
        img.size.w,
        img.size.h,
        format,
        Etc::ErrorMetric::RGBA,
        settings.quality,
        1,
        1,
        settings.mipmaps,
        0,
        images.data(),
        &encoding_time);

    printf("Encoded image to ETC2 in %i ms\n", encoding_time);

    ktx_uint32_t ktx_fmt = GL_COMPRESSED_RGBA8_ETC2_EAC;

    switch(format)
    {
    case format_t::R11:
        ktx_fmt = GL_COMPRESSED_R11_EAC;
        break;
    case format_t::RG11:
        ktx_fmt = GL_COMPRESSED_RG11_EAC;
        break;
    case format_t::RGB8:
        ktx_fmt = GL_COMPRESSED_RGB8_ETC2;
        break;
    default:
        break;
    }

    ktxTextureCreateInfo info = {
        .glInternalformat = ktx_fmt,
        .vkFormat         = 0,
        .pDfd             = nullptr,
        .baseWidth        = img.size.w,
        .baseHeight       = img.size.h,
        .baseDepth        = 1,
        .numDimensions    = 2,
        .numLevels        = settings.mipmaps,
        .numLayers        = 1,
        .numFaces         = 1,
        .isArray          = KTX_FALSE,
        .generateMipmaps  = KTX_FALSE,
    };

    ktxTexture1* texture{};

    auto error =
        ktxTexture1_Create(&info, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture);

    if(error != ktx_error_code_e::KTX_SUCCESS)
    {
        auto error_msg = magic_enum::enum_name(error);
        printf(
            "Failed to create KTX: %.*s\n",
            static_cast<int>(error_msg.size()),
            error_msg.data());
        return std::nullopt;
    }

    uint32_t i = 0;
    for(Etc::RawImage const& img : images)
    {
        auto error = ktxTexture_SetImageFromMemory(
            ktxTexture(texture),
            i++,
            0,
            0,
            img.paucEncodingBits.get(),
            img.uiEncodingBitsBytes);
        if(error != ktx_error_code_e::KTX_SUCCESS)
        {
            printf("Failed setting mipmap image %u\n", i);
            return std::nullopt;
        }
    }

    return texture;
}

} // namespace compressor::etc2

namespace compressor::bcn {

std::optional<ktxTexture1*> encode(
    compressor::rgba_image_t const& img,
    typing::pixels::CompFlags       format,
    typing::PixCmp                  channels,
    settings_t&&                    settings)
{
    using typing::pixels::CompFlags;
    ktx_uint32_t ktx_fmt = GL_COMPRESSED_RGBA8_ETC2_EAC;

    switch(format)
    {
    case CompFlags::BC1:
        ktx_fmt = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        break;
    case CompFlags::BC2:
        ktx_fmt = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case CompFlags::BC3:
        ktx_fmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    case CompFlags::BC4:
        ktx_fmt = GL_COMPRESSED_RED_RGTC1;
        break;
    case CompFlags::BC5:
        ktx_fmt = GL_COMPRESSED_RG_RGTC2;
        break;
    case CompFlags::BC6H:
        ktx_fmt = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
        break;
    case CompFlags::BC7:
        ktx_fmt = GL_COMPRESSED_RGBA_BPTC_UNORM;
        break;
    default:
        return std::nullopt;
    }

    ktxTextureCreateInfo info = {
        .glInternalformat = ktx_fmt,
        .vkFormat         = 0,
        .pDfd             = nullptr,
        .baseWidth        = img.size.w,
        .baseHeight       = img.size.h,
        .baseDepth        = 1,
        .numDimensions    = 2,
        .numLevels        = settings.mipmaps,
        .numLayers        = 1,
        .numFaces         = 1,
        .isArray          = KTX_FALSE,
        .generateMipmaps  = KTX_FALSE,
    };

    ktxTexture1* texture{};

    auto error =
        ktxTexture1_Create(&info, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture);

    if(error != ktx_error_code_e::KTX_SUCCESS)
    {
        auto error_msg = magic_enum::enum_name(error);
        printf(
            "Failed to create KTX: %.*s\n",
            static_cast<int>(error_msg.size()),
            error_msg.data());
        return std::nullopt;
    }

    u32 block_size = 4;
    u32 block_storage = 8;
    u32 stride = img.size.w * static_cast<u32>(img.bpp);
    std::function<void(void*, const u8*)> encoder;
    std::vector<u8> scratch;

    u32 level = rgbcx::MIN_LEVEL;
    bc7enc_compress_block_params block_params;
    bc7enc_compress_block_params_init(&block_params);
    bc7enc_compress_block_params_init_perceptual_weights(&block_params);

    rgbcx::init();
    using typing::pixels::CompFlags;
    switch(format)
    {
    case CompFlags::BC1:
    {
        scratch.resize(4 * 4 * 4); // 8-bit RGBA 4x4 blocks
        encoder = [level, stride, &scratch](void* dest, const u8* src) {
            for(u32 i = 0; i<4; ++i)
                memcpy(&scratch[4 * 4 * i], &src[stride * i], 4 * 4);
            rgbcx::encode_bc1(level, dest, scratch.data(), true, false);
        };
        break;
    }
    case CompFlags::BC2:
    case CompFlags::BC3:
    {
        block_storage = 16;
        scratch.resize(4 * 4 * 4); // 8-bit RGBA 4x4 blocks
        encoder = [level, stride, &scratch](void* dest, const u8* src) {
            for(u32 i = 0; i<4; ++i)
                memcpy(&scratch[4 * 4 * i], &src[stride * i], 4 * 4);
            rgbcx::encode_bc3(level, dest, scratch.data());
        };
        break;
    }
    case CompFlags::BC4:
    {
        if(channels != typing::PixCmp::R)
        {
            printf("%s\n", fmt::format("cannot map {} to {} for BC4",
                magic_enum::enum_name(channels),
                magic_enum::enum_name(typing::PixCmp::R)).c_str());
            return std::nullopt;
        }
        encoder = [stride](void* dest, const u8* src) {
            rgbcx::encode_bc4(dest, src, stride);
        };
        break;
    }
    case CompFlags::BC5:
    {
        if(channels != typing::PixCmp::RG)
        {
            printf("%s\n", fmt::format("cannot map {} to {} for BC5",
                magic_enum::enum_name(channels),
                magic_enum::enum_name(typing::PixCmp::RG)).c_str());
            return std::nullopt;
        }
        block_storage = 16;
        encoder = [stride](void* dest, const u8* src) {
            rgbcx::encode_bc5(dest, src, stride);
        };
        break;
    }
    case CompFlags::BC7:
    {
        bc7enc_compress_block_init();

        block_size = 16;
        block_storage = 16;
        scratch.resize(16 * 16 * 4); // 8-bit RGBA 16x16 blocks
        encoder = [&scratch, &block_params, stride](void* dest, const u8* src) {
            for(u32 i = 0; i<16; ++i)
                memcpy(&scratch[16 * 4 * i], &src[stride * i], 16 * 4);
            bc7enc_compress_block(dest, scratch.data(), &block_params);
        };
        break;
    }
    default:
    {
        return std::nullopt;
    }
    }

    u32 block_width = img.size.w / block_size;
    u32 block_height = img.size.h / block_size;

    /* TODO: Mipmapping */

    std::vector<u8> output(block_width * block_height * block_storage);
    auto const& size = img.size;
    for(u32 y = 0; y<img.size.h; y+=block_size)
        for(u32 x = 0; x<img.size.w; x+=block_size)
        {
            auto out_offset = block_width * (y / block_size) + x / block_size;
            out_offset *= block_storage;
            auto in_offset = size.w * y + x;
            in_offset *= 4;
            encoder(&output[out_offset], &img.data[in_offset]);
        }

    error = ktxTexture_SetImageFromMemory(
        ktxTexture(texture),
        0,
        0,
        0,
        output.data(),
        output.size());
    if(error != ktx_error_code_e::KTX_SUCCESS)
    {
        printf("Failed setting mipmap image %u\n", 0);
        return std::nullopt;
    }

    return texture;
}

} // namespace compressor::bcn
