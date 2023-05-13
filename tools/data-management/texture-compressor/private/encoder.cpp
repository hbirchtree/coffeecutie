#include "encoder.h"

#include <glad/gl.h>
#include <ktx.h>
#include <magic_enum.hpp>

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

    ktxTextureCreateInfo info = {
        .glInternalformat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
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

    auto error
        = ktxTexture1_Create(&info, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture);

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
