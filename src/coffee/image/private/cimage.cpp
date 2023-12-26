#include <coffee/image/cimage.h>

#include <coffee/core/CProfiling>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/range.h>

#include <coffee/core/CDebug>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_ABI "STB::"

using namespace semantic;
using namespace typing::pixels;

namespace Coffee {
namespace stb {

void _stbi_write_data(void* ctxt, void* data, int size)
{
    auto* target = C_RCAST<std::vector<char>*>(ctxt);

    semantic::Span<char> source(C_RCAST<char*>(data), size);
    std::copy(source.begin(), source.end(), std::back_inserter(*target));
}

namespace stb_templates {

template<typename PixType>
bool LoadFromMemory(BytesConst const&, image<PixType>*, int)
{
    return false;
}

template<>
bool LoadFromMemory(BytesConst const& src, image_rw* target, int req_comp)
{
    target->data = stbi_load_from_memory(
        C_RCAST<const byte_t*>(src.data),
        C_FCAST<int>(src.size),
        C_RCAST<i32*>(&target->size.w),
        C_RCAST<i32*>(&target->size.h),
        &target->bpp,
        req_comp);

    return target->data;
}

template<>
bool LoadFromMemory(BytesConst const& src, image_float* target, int req_comp)
{
    target->data = stbi_loadf_from_memory(
        C_RCAST<const byte_t*>(src.data),
        C_FCAST<int>(src.size),
        C_RCAST<i32*>(&target->size.w),
        C_RCAST<i32*>(&target->size.h),
        &target->bpp,
        req_comp);

    return target->data;
}

/*!
 * \brief NearestNeighborResize
 * \param src
 * \param out
 * \param srcSize size of src image, must be larger than output image
 * \param outSize
 */
template<typename PixType>
static void NearestNeighborResize(
    rgba_untyped<PixType> const* src,
    rgba_untyped<PixType>*       out,
    Size const&                  srcSize_,
    Size const&                  outSize_)
{
    size_2d<u32> srcSize = srcSize_.convert<u32>();
    size_2d<u32> outSize = outSize_.convert<u32>();

    const auto pixel_ratio_w = srcSize.w / outSize.h;
    const auto pixel_ratio_h = srcSize.h / outSize.h;

    for(auto y : stl_types::Range<>(outSize.h))
        for(auto x : stl_types::Range<>(outSize.w))
        {
            auto src_x = x * pixel_ratio_w;
            auto src_y = y * pixel_ratio_h;

            auto&       outPix = out[(y * outSize.w + x) * sizeof(PixType)];
            auto const& srcPix
                = src[(src_y * srcSize.w + src_x) * sizeof(PixType)];

            outPix = srcPix;
        }
}

template<typename PixType>
bool ResizeImage(
    image<PixType> const&, Size const&, image<PixType>&, int, stb_error&)
{
    return false;
}

template<>
bool ResizeImage(
    image<f32> const& img,
    Size const&       target,
    image<f32>&       data,
    int               req_comp,
    stb_error&        ec)
{
    auto res = stbir_resize_float(
        img.data,
        img.size.w,
        img.size.h,
        0,
        data.data,
        target.w,
        target.h,
        0,
        req_comp);

    if(res == 0)
        ec = STBError::ResizeError;

    return true;
}

template<>
bool ResizeImage(
    image<u8> const& img,
    Size const&      target,
    image<u8>&       data,
    int              req_comp,
    stb_error&       ec)
{
    auto res = stbir_resize_uint8_generic(
        img.data,
        img.size.w,
        img.size.h,
        0,
        data.data,
        target.w,
        target.h,
        0,
        req_comp,
        3,
        0,
        STBIR_EDGE_CLAMP,
        STBIR_FILTER_DEFAULT,
        STBIR_COLORSPACE_LINEAR,
        nullptr);

    if(res == 0)
        ec = STBError::ResizeError;

    return true;
}

template<typename PixType>
bool LoadData(
    image<PixType>* target, BytesConst const& src, stb_error& ec, PixCmp comp)
{
    DProfContext _(STB_ABI "Loading image");

    int scomp = STBI_rgb_alpha;

    switch(comp)
    {
    case PixCmp::R:
        scomp = STBI_grey;
        break;
    case PixCmp::RG:
        scomp = STBI_grey_alpha;
        break;
    case PixCmp::RGB:
        scomp = STBI_rgb;
        break;
    case PixCmp::RGBA:
        scomp = STBI_rgb_alpha;
        break;
    default:
        ec = STBError::InvalidComponents;
        return false;
    }

    if(!stb_templates::LoadFromMemory(src, target, scomp))
    {
        ec = STBError::DecodingError;
        ec = stbi_failure_reason();
        return false;
    }

    target->bpp        = scomp;
    target->data_owner = semantic::mem_chunk<libc_types::u8>::ofBytes(
        target->data, C_FCAST<szptr>(target->size.area() * target->bpp));

    if(!target->data)
    {
        ec = STBError::DecodingError;
        ec = stbi_failure_reason();
        return false;
    } else
    {
        auto src           = target->data_owner.view;
        target->data_owner = semantic::mem_chunk<libc_types::u8>::withSize(
            target->size.area() * target->bpp * sizeof(PixType));
        std::copy(
            src.begin(),
            src.end(),
            target->data_owner.view.begin());
        target->data = reinterpret_cast<PixType*>(target->data_owner.data);
        return true;
    }
}

template<typename PixType>
image<PixType> Resize(
    image<PixType> const& img, const Size& target, int channels, ImageHint hint)
{
    using namespace ::enum_helpers;

    Bytes          data = Bytes::withSize(target.area() * channels);
    image<PixType> out_image;
    stb_error      ec;

    bool sharpen = !feval(hint, ImageHint::NormalMap);

    out_image = image<PixType>::From(std::move(data), target, channels);

    /* For smaller images, nearest-neighbor scaling preserves more detail */
    if(target.w > 96 || !sharpen)
    {
        if(!stb_templates::ResizeImage(img, target, out_image, channels, ec))
            ec = STBError::InvalidPixFmt;
    } else
    {
        stb_templates::NearestNeighborResize(
            C_RCAST<rgba_untyped<PixType>*>(img.data),
            C_RCAST<rgba_untyped<PixType>*>(out_image.data),
            img.size,
            target);
    }

    return out_image;
}

} // namespace stb_templates

/*
static void ReshapeRGBA(Bytes& src, szptr numPixels, u32 channels)
{
    auto outData = Bytes::Alloc(numPixels * sizeof(rgba_t));

    auto srcImage = src.as<rgba_t>();
    auto dstImage = outData.as<rgba_t>();

    for(auto i : Range<>(numPixels))
    {
        rgba_t& srcPix = *C_RCAST<rgba_t*>(&srcImage[i * channels]);
        rgba_t& p      = dstImage[i];

        switch(channels)
        {
        case 1:
        {
            p.r = srcPix.r;
            p.g = srcPix.r;
            p.b = srcPix.r;
            p.a = 0xFF;
            break;
        }
        case 2:
        {
            p.r = srcPix.r;
            p.g = srcPix.r;
            p.b = srcPix.r;
            p.a = srcPix.g;
            break;
        }
        case 3:
        {
            p.r = srcPix.r;
            p.g = srcPix.g;
            p.b = srcPix.b;
            p.a = 0xFF;
            break;
        }
        default:
            break;
        }
    }

    src = std::move(outData);
}
*/

/* Instantiate some templates as symbols here */
bool LoadData(
    image<u8>* target, BytesConst const& src, stb_error& ec, PixCmp comp)
{
    return stb_templates::LoadData(target, src, ec, comp);
}
bool LoadData(
    image<f32>* target, BytesConst const& src, stb_error& ec, PixCmp comp)
{
    return stb_templates::LoadData(target, src, ec, comp);
}
image<u8> Resize(
    image<u8> const& img, const Size& target, int channels, ImageHint hint)
{
    return stb_templates::Resize(img, target, channels, hint);
}
image<f32> Resize(image<f32> const& img, const Size& target, int channels)
{
    return stb_templates::Resize(img, target, channels, ImageHint::Undefined);
}

bool SavePNG(Bytes& target, const image_const& src, stb_error& ec)
{
    DProfContext _(STB_ABI "Saving PNG image");

    target = {};

    auto res = stbi_write_png_to_func(
        _stbi_write_data,
        &target.allocation,
        src.size.w,
        src.size.h,
        src.bpp,
        src.data,
        src.size.w * src.bpp);
    target.updatePointers(semantic::Ownership::Owned);

    if(!res)
    {
        ec = STBError::EncodingError;
        ec = stbi_failure_reason();
    }

    return res;
}

bool SaveTGA(Bytes& target, const image_const& src, stb_error& ec)
{
    DProfContext _(STB_ABI "Saving TGA image");

    target = {};

    auto res = stbi_write_tga_to_func(
        _stbi_write_data,
        &target.allocation,
        src.size.w,
        src.size.h,
        src.bpp,
        src.data);
    target.updatePointers(semantic::Ownership::Owned);

    if(!res)
    {
        ec = STBError::EncodingError;
        if(auto err = stbi_failure_reason())
            ec = err;
    }

    return res;
}

bool SaveJPG(Bytes& target, const image_const& src, stb_error& ec, int qual)
{
    DProfContext _(STB_ABI "Saving JPG image");

    target = {};

    auto res = stbi_write_jpg_to_func(
        _stbi_write_data,
        &target.allocation,
        src.size.w,
        src.size.h,
        src.bpp,
        src.data,
        qual);
    target.updatePointers(semantic::Ownership::Owned);

    if(!res)
    {
        ec = STBError::EncodingError;
        if(auto err = stbi_failure_reason())
            ec = err;
    }

    return res;
}

image_float ToFloat(const image_const& image)
{
    image_float out = image_float::From(
        Bytes::withSize(
            C_FCAST<szptr>(image.size.area() * image.bpp) * sizeof(f32)),
        image.size,
        image.bpp);

    using source_type = u8;
    using target_type = f32;

    source_type const* src_ptr = image.data;
    target_type*       out_ptr = out.data;

    szptr components = C_FCAST<szptr>(image.bpp);

    for(auto i : Range<>(C_FCAST<szptr>(image.size.area()) * components))
    {
        out_ptr[i] = target_type(src_ptr[i]) / 255.f;
    }

    return out;
}

const char* stb_error_category::name() const noexcept
{
    return "stb";
}

std::string stb_error_category::message(int error_code) const
{
    switch(C_CAST<STBError>(error_code))
    {
    case STBError::GeneralError:
        return "General error";
    case STBError::DecodingError:
        return "Decoding error";
    case STBError::EncodingError:
        return "Encoding error";
    case STBError::ResizeError:
        return "Resizing error";
    case STBError::InvalidComponents:
        return "Invalid components specified";
    case STBError::InvalidPixFmt:
        return "Invalid pixel format specified";
    }

    throw implementation_error("unimplemented error message");
}

} // namespace stb

Bytes TGA::Save(stb::image_const const& im, stb::stb_error& ec)
{
    Bytes output;
    stb::SaveTGA(output, im, ec);
    return output;
}

Bytes PNG::Save(stb::image_const const& im, stb::stb_error& ec)
{
    Bytes output;
    Stb::SavePNG(output, im, ec);
    return output;
}

Bytes JPG::Save(const stb::image_const& src, stb_error& ec, int qual)
{
    Bytes output;
    stb::SaveJPG(output, src, ec, qual);
    return output;
}

} // namespace Coffee
