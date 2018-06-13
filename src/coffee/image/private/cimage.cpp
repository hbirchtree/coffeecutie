#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>
#include <coffee/core/types/cdef/memsafe.h>
#include <coffee/image/cimage.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_ABI "STB::"

namespace Coffee {
namespace stb {

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

bool LoadData(
    image_rw*         target,
    BytesConst const& src,
    stb_error&        ec,
    PixelComponents   comp)
{
    DProfContext _(STB_ABI "Loading image");

    int scomp = STBI_rgb_alpha;

    switch(comp)
    {
    case PixelComponents::R:
        scomp = STBI_grey;
        break;
    case PixelComponents::RG:
        scomp = STBI_grey_alpha;
        break;
    case PixelComponents::RGB:
        scomp = STBI_rgb;
        break;
    case PixelComponents::RGBA:
        scomp = STBI_rgb_alpha;
        break;
    default:
        ec = STBError::InvalidComponents;
        return false;
    }

    target->data = stbi_load_from_memory(
        C_RCAST<const byte_t*>(src.data),
        C_FCAST<int>(src.size),
        &target->size.w,
        &target->size.h,
        &target->bpp,
        scomp);

    target->bpp = scomp;

    if(!target->data)
    {
        ec = STBError::DecodingError;
        ec = stbi_failure_reason();
    } else
        return true;

    return target->data != nullptr;
}

void _stbi_write_data(void* ctxt, void* data, int size)
{
    Bytes* target = C_FCAST<Bytes*>(ctxt);
    auto   offset = target->size;
    target->size += C_FCAST<szptr>(size);

    if(!target->data)
        *target = Bytes::Alloc(target->size);
    else
        target->resize(target->size);

    MemCpy(Bytes::From(data, size), target->at(offset));
}

/*!
 * \brief NearestNeighborResize
 * \param src
 * \param out
 * \param srcSize size of src image, must be larger than output image
 * \param outSize
 */
static void NearestNeighborResize(
    rgba_t const* src, rgba_t* out, Size const& srcSize_, Size const& outSize_)
{
    _cbasic_size_2d<u32> srcSize = srcSize_.convert<u32>();
    _cbasic_size_2d<u32> outSize = outSize_.convert<u32>();

    const auto pixel_ratio_w = srcSize.w / outSize.h;
    const auto pixel_ratio_h = srcSize.h / outSize.h;

    for(auto y : Range<>(outSize.h))
        for(auto x : Range<>(outSize.w))
        {
            auto src_x = x * pixel_ratio_w;
            auto src_y = y * pixel_ratio_h;

            auto&       outPix = out[y * outSize.w + x];
            auto const& srcPix = src[src_y * srcSize.w + src_x];

            outPix = srcPix;
        }
}

Bytes Resize(image_const const& img, const CSize& target, int channels)
{
    Bytes data = Bytes::Alloc(target.area() * channels);

    if(target.area() > img.size.area() || channels != 4)
        stbir_resize_uint8(
            img.data,
            img.size.w,
            img.size.h,
            0,
            data.data,
            target.w,
            target.h,
            0,
            channels);
    else
        NearestNeighborResize(
            C_RCAST<const rgba_t*>(img.data),
            C_RCAST<rgba_t*>(data.data),
            img.size,
            target);

    return data;
}

bool SavePNG(Bytes& target, const image_const& src, stb_error& ec)
{
    DProfContext _(STB_ABI "Saving PNG image");

    auto res = stbi_write_png_to_func(
        _stbi_write_data,
        &target,
        src.size.w,
        src.size.h,
        src.bpp,
        src.data,
        src.size.w * src.bpp);

    if(res == 0)
    {
        ec = STBError::EncodingError;
        ec = stbi_failure_reason();
    }

    return res;
}

bool SaveTGA(Bytes& target, const image_const& src, stb_error& ec)
{
    DProfContext _(STB_ABI "Saving TGA image");

    auto res = stbi_write_tga_to_func(
        _stbi_write_data, &target, src.size.w, src.size.h, src.bpp, src.data);

    if(res == 0)
    {
        ec = STBError::EncodingError;
        ec = stbi_failure_reason();
    }

    return res;
}

bool SaveJPG(Bytes& target, const image_const& src, stb_error& ec, int qual)
{
    DProfContext _(STB_ABI "Saving JPG image");

    auto res = stbi_write_jpg_to_func(
        _stbi_write_data,
        &target,
        src.size.w,
        src.size.h,
        src.bpp,
        src.data,
        qual);

    if(res == 0)
    {
        ec = STBError::EncodingError;
        ec = stbi_failure_reason();
    }

    return res;
}

void ImageFreePtr(void* img)
{
    stbi_image_free(img);
}

void ImageFree(CStbImage* img)
{
    ImageFreePtr(img->data);
}

void DataSetDestr(Bytes& b)
{
    Bytes::SetDestr(b, [](Bytes& b) {
        //        ImageFreePtr(b.data);
        CFree(b.data);
        b.data     = nullptr;
        b.size     = 0;
        b.elements = 0;
    });
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
    case STBError::InvalidComponents:
        return "Invalid components specified";
    }
}

} // namespace stb

Bytes TGA::Save(stb::image_const const& im, stb::stb_error& ec)
{
    Bytes output;

    stb::SaveTGA(output, im, ec);
    stb::DataSetDestr(output);

    return output;
}

Bytes PNG::Save(stb::image_const const& im, stb::stb_error& ec)
{
    Bytes output;

    Stb::SavePNG(output, im, ec);
    stb::DataSetDestr(output);

    return output;
}

Bytes JPG::Save(const stb::image_const& src, stb_error& ec, int qual)
{
    Bytes output;

    bool stat = stb::SaveJPG(output, src, ec, qual);
    stb::DataSetDestr(output);

    return output;
}

} // namespace Coffee
