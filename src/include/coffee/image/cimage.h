#ifndef CIMAGE_LIB
#define CIMAGE_LIB

#include <coffee/core/CFiles>
#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/edef/pixenum.h>

namespace Coffee {
namespace IMG {

/*!
 * \brief For the cases when you need to store an image descriptor
 *  on disk, does not have any assumptions on format.
 */
struct serial_image
{
    static constexpr const u32 hard_signature = 0xBEEF1B01;

    u32                  signature = hard_signature;
    _cbasic_size_2d<u32> size;

    CompFlags comp_fmt;
    PixFmt    fmt;
    BitFmt    bit_fmt;
    u8        _pad;

    bool valid() const
    {
        return signature == hard_signature;
    }
};

} // namespace IMG

/*!
 * \brief Wrappers around stb_image for C++, not very abstract
 */
namespace stb {

enum class STBError
{
    GeneralError = 1,
    InvalidComponents,
    InvalidPixelFormat,
    DecodingError,
    EncodingError,
    ResizeError,
};

struct stb_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using stb_error = domain_error_code<STBError, stb_error_category>;

template<typename PixType>
struct image
{
    Bytes data_owner;

    PixType* data;
    Size     size;
    int      bpp;

    STATICINLINE image<PixType> From(void* data, Size const& size, int bpp = 4)
    {
        image<PixType> img;
        img.data = C_FCAST<PixType*>(data);
        img.size = size;
        img.bpp  = bpp;

        return img;
    }

    STATICINLINE image<PixType> From(
        Bytes const& data, Size const& size, int bpp = 4)
    {
        image<PixType> img;
        img.data = C_FCAST<PixType*>(data.data);
        img.size = size;
        img.bpp  = bpp;

        return img;
    }

    STATICINLINE image<PixType> From(
        Bytes&& data, Size const& size, int bpp = 4)
    {
        image<PixType> img;
        img.data_owner = std::move(data);
        img.data = C_FCAST<PixType*>(img.data_owner.data);
        img.size = size;
        img.bpp  = bpp;

        return img;
    }

    template<
        typename PixType2,
        typename std::enable_if<std::is_same<
            typename std::remove_cv<PixType>::type,
            typename std::remove_cv<PixType2>::type>::value>::type* = nullptr>
    operator image<PixType2>()
    {
        image<PixType2> out;

        out.data = C_RCAST<PixType2*>(this->data);
        out.size = this->size;
        out.bpp  = this->bpp;

        return out;
    }

    operator Bytes()
    {
        Bytes out;

        out.data     = C_RCAST<byte_t*>(this->data);
        out.size     = C_FCAST<szptr>(size.area() * bpp);
        out.elements = C_FCAST<szptr>(size.area());

        return out;
    }

    void disown()
    {
        data_owner = Bytes();
    }
};

using image_rw    = image<u8>;
using image_const = image<u8 const>;

using image_float       = image<scalar>;
using image_float_const = image<scalar>;

using CStbImage      = image_rw;
using CStbImageConst = image_const;

extern bool LoadData(
    image<u8>*        target,
    BytesConst const& src,
    stb_error&        ec,
    PixCmp            comp = PixCmp::RGBA);
extern bool LoadData(
    image<scalar>*    target,
    BytesConst const& src,
    stb_error&        ec,
    PixCmp            comp = PixCmp::RGBA);
/*!
 * \brief Resize an image using STB
 * \param img Target image
 * \param target New resolution
 * \param channels Amount of channels
 * \return True if success
 */
extern image<u8> Resize(image<u8> const& img, const Size& target, int channels);
extern image<scalar> Resize(
    image<scalar> const& img, const Size& target, int channels);

STATICINLINE bool LoadData(
    image_rw* target, BytesConst const& src, PixCmp comp = PixCmp::RGBA)
{
    stb_error ec;
    return LoadData(target, src, ec, comp);
}

STATICINLINE bool LoadData(
    image_rw* target, Bytes const& src, PixCmp comp = PixCmp::RGBA)
{
    stb_error ec;
    return LoadData(target, C_OCAST<BytesConst>(src), ec, comp);
}

/*!
 * \brief Convert an image from u8 components to floating-point
 * \param image
 * \return
 */
extern image_float ToFloat(image_const const& image);

/*!
 * \brief Save STB image to PNG
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool SavePNG(Bytes& target, const image_const& src, stb_error& ec);
/*!
 * \brief Save STB image to TGA
 * \param target Target resource
 * \param src STB image to save
 * \param ec
 * \return
 */
extern bool SaveTGA(Bytes& target, const image_const& src, stb_error& ec);
/*!
 * \brief Save STB image to JPG
 * \param target
 * \param src
 * \param ec
 * \param qual
 * \return
 */
extern bool SaveJPG(
    Bytes& target, const image_const& src, stb_error& ec, int qual = 80);
/*!
 * \brief Free image data
 * \param img
 */
extern void ImageFree(CStbImage* img);
extern void ImageFreePtr(void* img);

} // namespace stb

/*!
 * \brief Wrapping functions for simpler usage
 */
namespace IMG {
using stb::stb_error;

STATICINLINE bool Load(
    Bytes& r, PixCmp cmp, BitFmt& fmt, Bytes& data, CSize& res)
{
    stb_error ec;

    stb::image_rw img;
    bool          stat = stb::LoadData(&img, C_OCAST<BytesConst>(r), ec, cmp);

    img.disown();

    fmt  = BitFmt::UByte;
    data = C_OCAST<Bytes>(img);
    res  = img.size;
    Bytes::SetDestr(data, [](Bytes& b) { stb::ImageFreePtr(b.data); });

    return stat;
}

STATICINLINE bool Load(
    Bytes&& r, PixCmp cmp, BitFmt& fmt, Bytes& data, CSize& res)
{
    return Load(r, cmp, fmt, data, res);
}

} // namespace IMG

namespace PNG {
using stb::stb_error;
/*!
 * \brief Save image data from RGBA8 format into PNG data
 * \param src
 * \param res
 * \return
 */
extern Bytes Save(stb::image_const const& im, stb_error& ec);

STATICINLINE Bytes Save(stb::image_const const& im)
{
    stb_error ec;
    return Save(im, ec);
}

} // namespace PNG

namespace TGA {
using stb::stb_error;
/*!
 * \brief Save image data from RGBA8, RGB8, RG8 or R8 into TGA data
 * \param data
 * \param res
 * \param bpp
 * \return
 */
extern Bytes Save(stb::image_const const& im, stb_error& ec);

STATICINLINE Bytes Save(stb::image_const const& im)
{
    stb_error ec;
    return Save(im, ec);
}
} // namespace TGA

namespace JPG {
using stb::stb_error;
extern Bytes Save(stb::image_const const& src, stb_error& ec, int qual = 80);

STATICINLINE Bytes Save(stb::image_const const& src, int qual = 80)
{
    stb_error ec;
    return Save(src, ec, qual);
}
} // namespace JPG

namespace Stb {
using namespace stb;
using Img      = CStbImage;
using ImgConst = CStbImageConst;
} // namespace Stb

} // namespace Coffee

#endif
