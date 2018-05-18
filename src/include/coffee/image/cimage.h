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

template<typename PixType>
struct image
{
    PixType* data;
    Size     size;
    int      bpp;

    STATICINLINE image<PixType> From(void* data, Size const& size, int bpp = 4)
    {
        image<PixType> img;
        img.data = data;
        img.size = size;
        img.bpp  = bpp;

        return img;
    }

    STATICINLINE image<PixType> From(
        Bytes const& data, Size const& size, int bpp = 4)
    {
        image<PixType> img;
        img.data = data.data;
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
        out.size     = size.area() * bpp;
        out.elements = size.area();

        return out;
    }
};

using image_rw    = image<u8>;
using image_const = image<u8 const>;

using CStbImage      = image_rw;
using CStbImageConst = image_const;

/*!
 * \brief Print any potential STB errors
 */
extern void Error();

extern bool LoadData(
    image_rw* target, BytesConst const& src, PixCmp comp = PixCmp::RGBA);

STATICINLINE bool LoadData(
    image_rw* target, Bytes const& src, PixCmp comp = PixCmp::RGBA)
{
    return LoadData(target, C_OCAST<BytesConst>(src), comp);
}

/*!
 * \brief Resize an image using STB
 * \param img Target image
 * \param target New resolution
 * \param channels Amount of channels
 * \return True if success
 */
extern Bytes Resize(image_const const& img, const CSize& target, int channels);
/*!
 * \brief Save STB image to PNG file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool SavePNG(Bytes& target, const image_const& src);
/*!
 * \brief Save STB image to TGA file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool SaveTGA(Bytes& target, const image_const& src);

extern bool SaveJPG(Bytes& target, const image_const& src, int qual = 80);
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

STATICINLINE bool Load(
    Bytes& r, PixCmp cmp, BitFmt& fmt, Bytes& data, CSize& res)
{
    stb::image_rw img;
    bool          stat = stb::LoadData(&img, C_OCAST<BytesConst>(r), cmp);

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
/*!
 * \brief Save image data from RGBA8 format into PNG data
 * \param src
 * \param res
 * \return
 */
extern Bytes Save(stb::image_const const& im);

} // namespace PNG

namespace TGA {
/*!
 * \brief Save image data from RGBA8, RGB8, RG8 or R8 into TGA data
 * \param data
 * \param res
 * \param bpp
 * \return
 */
extern Bytes Save(Bytes const& data, CSize const& res, int bpp = 4);
} // namespace TGA

namespace JPG {
extern Bytes Save(stb::image_const const& src, int qual = 80);
}

namespace Stb {
using namespace stb;
using Img      = CStbImage;
using ImgConst = CStbImageConst;
} // namespace Stb

} // namespace Coffee

#endif
