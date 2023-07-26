#include <glw/texture_formats.h>

#include <coffee/image/ktx_load.h>

#include <ktx.h>
#include <ktxvulkan.h>

#include <peripherals/stl/range.h>

void ktxDeleter::operator()(ktxTexture* ptr)
{
    ktxTexture_Destroy(ptr);
}

namespace ktx {

stl_types::result<texture_t, std::string> load_from(
    semantic::Span<const char>&& data)
{
    ktxTexture* texture{};
    auto        ec = ktxTexture_CreateFromMemory(
        reinterpret_cast<const ktx_uint8_t*>(data.data()),
        data.size(),
        KTX_TEXTURE_CREATE_ALLOC_STORAGE,
        &texture);
    if(ec != KTX_SUCCESS)
        return ktxErrorString(ec);

    texture_t out;
    out.texture = std::unique_ptr<ktxTexture, ktxDeleter>(texture);
    out.levels  = texture->numLevels;
    out.format  = gl::tex::format_of(
        static_cast<gl::tex::vk_format_t>(ktxTexture_GetVkFormat(texture)));

    auto base_ptr = ktxTexture_GetData(texture);
    for(auto i : stl_types::Range<>(texture->numLevels))
    {
        out.mips.emplace_back();
        texture_t::mipmap_t& mip = out.mips.back();

        ktx_size_t offset;
        ktxTexture_GetImageOffset(texture, i, 0, 0, &offset);
        ktx_size_t size = ktxTexture_GetImageSize(texture, i);
        mip.data        = semantic::Span<const char>(
            reinterpret_cast<const char*>(base_ptr + offset), size);
        mip.size.x = texture->baseWidth >> i;
        mip.size.y = texture->baseHeight >> i;
        if(texture->isArray || texture->isCubemap)
            mip.size.z = texture->baseDepth;
        else
            mip.size.z = texture->baseDepth >> i;
    }
    return out;
}

} // namespace ktx
