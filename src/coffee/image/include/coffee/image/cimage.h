#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/libc_types.h>
#include <coffee/core/types/size.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>
#include <peripherals/typing/pixels/rgba.h>

namespace Coffee {
namespace IMG {

using namespace typing::pixels;
using typing::PixCmp;
using semantic::Bytes;
using semantic::BytesConst;

/*!
 * \brief For the cases when you need to store an image descriptor
 *  on disk, does not have any assumptions on format.
 */
struct serial_image
{
    serial_image() : size()
    {
        v2.format  = {};
        v2.bit_fmt = BitFmt::Byte;
    }

    static serial_image const* From(Bytes const& data)
    {
        if(data.size < 8)
            return nullptr;

        serial_image const* img = C_RCAST<serial_image const*>(data.data);

        if(!img->valid())
            return nullptr;

        if(img->desc_size > 0 && img->desc_size < 128)
            return img;

        return nullptr;
    }

    static constexpr const u32 hard_signature = 0xBEEF1B01;

    const u32 signature = hard_signature;
    const u16 desc_size = sizeof(serial_image);
    const u16 desc_vers = 2;

    size_2d<u32> size;

    union
    {
        struct
        {
            CompFlags comp_fmt;
            PixFmt    fmt;
            BitFmt    bit_fmt;
            u8        pad_;
        } v1;
        struct
        {
            CompFmt format;
            BitFmt  bit_fmt;
            u8      pad_;
        } v2;
        struct
        {
            u32 v1;
            u32 v2;
        } pad;
    };

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

using namespace typing::pixels;
using typing::PixCmp;
using semantic::Bytes;
using semantic::BytesConst;

enum class ImageHint : u16
{
    Undefined = 0x0,
    NormalMap = 0x1,
};

C_FLAGS(ImageHint, u16);

enum class STBError
{
    GeneralError = 1,
    InvalidComponents,
    InvalidPixFmt,
    DecodingError,
    EncodingError,
    ResizeError,
};

struct stb_error_category : std::error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using stb_error = stl_types::domain_error_code<STBError, stb_error_category>;

template<typename PixType>
struct image
{
    semantic::Bytes data_owner;

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
        img.data       = C_RCAST<PixType*>(img.data_owner.data);
        img.size       = size;
        img.bpp        = bpp;

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
        return Bytes::ofBytes(this->data, size.area() * bpp);
    }

    void disown()
    {
        data_owner = Bytes();
    }
};

using image_rw    = image<u8>;
using image_const = image<u8 const>;

using image_float       = image<f32>;
using image_float_const = image<f32 const>;

using CStbImage      = image_rw;
using CStbImageConst = image_const;

extern bool LoadData(
    image<u8>*        target,
    BytesConst const& src,
    stb_error&        ec,
    PixCmp            comp = PixCmp::RGBA);
extern bool LoadData(
    image<f32>*    target,
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
extern image<u8> Resize(
    image<u8> const& img,
    const Size&      target,
    int              channels,
    ImageHint        hint = ImageHint::Undefined);
extern image<f32> Resize(
    image<f32> const& img, const Size& target, int channels);

inline bool LoadData(
    image_rw* target, BytesConst const& src, PixCmp comp = PixCmp::RGBA)
{
    stb_error ec;
    auto      out = LoadData(target, src, ec, comp);
    C_ERROR_CHECK(ec)
    return out;
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

} // namespace stb

/*!
 * \brief Wrapping functions for simpler usage
 */
namespace IMG {
using stb::stb_error;

inline bool Load(BytesConst& r, PixCmp cmp, BitFmt& fmt, Bytes& data, Size& res)
{
    stb_error ec;

    stb::image_rw img;
    bool          stat = stb::LoadData(&img, r, ec, cmp);
    C_ERROR_CHECK(ec)

    fmt  = BitFmt::UByte;
    data = std::move(img.data_owner);
    res  = img.size;

    img.disown();

    return stat;
}

inline bool Load(
    BytesConst&& r, PixCmp cmp, BitFmt& fmt, Bytes& data, Size& res)
{
    return Load(r, cmp, fmt, data, res);
}

} // namespace IMG

namespace PNG {
using semantic::Bytes;
using stb::stb_error;

/*!
 * \brief Save image data from RGBA8 format into PNG data
 * \param src
 * \param res
 * \return
 */
extern Bytes Save(stb::image_const const& im, stb_error& ec);

inline Bytes Save(stb::image_const const& im)
{
    stb_error ec;
    return Save(im, ec);
}

} // namespace PNG

namespace TGA {

using semantic::Bytes;
using stb::stb_error;
/*!
 * \brief Save image data from RGBA8, RGB8, RG8 or R8 into TGA data
 * \param data
 * \param res
 * \param bpp
 * \return
 */
extern Bytes Save(stb::image_const const& im, stb_error& ec);

inline Bytes Save(stb::image_const const& im)
{
    stb_error ec;
    return Save(im, ec);
}
} // namespace TGA

namespace JPG {
using semantic::Bytes;
using stb::stb_error;

extern Bytes Save(stb::image_const const& src, stb_error& ec, int qual = 80);

inline Bytes Save(stb::image_const const& src, int qual = 80)
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
