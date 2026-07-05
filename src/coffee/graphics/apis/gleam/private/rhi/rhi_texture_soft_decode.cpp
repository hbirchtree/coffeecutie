#include "task.h"
#include <coffee/graphics/apis/gleam/rhi_texture.h>
#include <peripherals/constants.h>
#include <peripherals/libc/types.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <functional>
#include <peripherals/stl/range.h>

#if defined(GLEAM_ENABLE_SOFTWARE_BCN)
#define BCDEC_IMPLEMENTATION
#define BCDEC_STATIC
#include <bcdec.h>
#endif

#if defined(GLEAM_ENABLE_SOFTWARE_PVRTC)
#include <pvrtcdec/PVRTDecompress.h>
#endif

#define NOT_SUPPORTED " not supported by hardware and no software fallback"

#include <coffee/core/debug/formatting.h>

namespace gleam {

struct texture_decode_not_available : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct texture_decode_not_implemented : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

using typing::pixels::pix_fmt;

bool texture_t::requires_software_decode()
{
    using Comp = typing::pixels::comp_flags;

    if(m_format.pixfmt == pix_fmt::BCn)
    {
        const auto s3tc_support = m_features.tex.ext.s3tc;
        const auto rgtc_support = m_features.tex.gl.rgtc ||
                                  m_features.tex.arb.rgtc ||
                                  m_features.tex.ext.rgtc;
        const auto bptc_support = m_features.tex.gl.bptc ||
                                  m_features.tex.arb.bptc ||
                                  m_features.tex.ext.bptc;
#if defined(GLEAM_ENABLE_SOFTWARE_BCN)
        if(stl_types::any_of(
               m_format.cmpflg, Comp::BC1, Comp::BC2, Comp::BC3) &&
           !s3tc_support)
            return true;
        if(stl_types::any_of(m_format.cmpflg, Comp::BC4, Comp::BC5) &&
           !rgtc_support)
            return true;
        if(stl_types::any_of(m_format.cmpflg, Comp::BC6H, Comp::BC7) &&
           !bptc_support)
            return true;
#else
        Throw(texture_decode_not_available("BCn" NOT_SUPPORTED));
#endif
    }
#if defined(GLEAM_ENABLE_SOFTWARE_PVRTC)
    if(m_format.pixfmt == pix_fmt::ETC1 && !m_features.tex.oes.etc1)
        return true;
    if(m_format.pixfmt == pix_fmt::PVRTC && !m_features.tex.img.pvrtc)
        return true;
#else
    if(m_format.pixfmt == pix_fmt::ETC1 && !m_features.tex.oes.etc1)
        Throw(texture_decode_not_available("ETC1" NOT_SUPPORTED));
    if(m_format.pixfmt == pix_fmt::PVRTC && !m_features.tex.img.pvrtc)
        Throw(texture_decode_not_available("PVRTC" NOT_SUPPORTED));
#endif
    // WebGL2 reads RGB565 uploads via HEAPU16[ptr >>> 1]. If the pixel data
    // is at an odd byte offset in the WASM heap the shift truncates the low
    // bit, swapping bytes within every u16 and producing wrong colors.
    // Decode to RGBA8 in C++ first so the upload pointer is always aligned.
    if(m_format.pixfmt == pix_fmt::RGB565 && m_workarounds.tex.requires_aligned)
        return true;

    if(m_format.pixfmt == pix_fmt::ASTC && !m_features.tex.gl.astc &&
       !m_features.tex.khr.astc)
        Throw(texture_decode_not_available("ASTC" NOT_SUPPORTED));
    if(m_format.pixfmt == pix_fmt::ETC2 && !m_features.tex.gl.etc2)
        Throw(texture_decode_not_available("ETC2" NOT_SUPPORTED));

    return false;
}

std::optional<PixDesc> texture_t::software_decode_format()
{
    if(m_format.pixfmt == pix_fmt::BCn && requires_software_decode())
        return PixDesc(pix_fmt::RGBA8);
    return std::nullopt;
}

#if defined(GLEAM_ENABLE_SOFTWARE_BCN)
static std::vector<char> software_decode_bcn(
    PixDesc const&               m_format,
    semantic::Span<const char>&& data,
    size_3d<i32> const&          size)
{
    using typing::pixels::comp_flags;
    using bcdec_transform_t     = void (*)(const void*, void*, int);
    auto              out_size  = size.volume() * 4u;
    auto              out_pitch = size.w * 4u;
    std::vector<char> out(out_size, 0);
    bcdec_transform_t transform = nullptr;
    auto              blk_size  = BCDEC_BC1_BLOCK_SIZE;

    switch(m_format.cmpflg)
    {
    case comp_flags::BC1:
        transform = bcdec_bc1;
        break;
    case comp_flags::BC2:
        transform = bcdec_bc2;
        blk_size  = BCDEC_BC2_BLOCK_SIZE;
        break;
    case comp_flags::BC3:
        transform = bcdec_bc3;
        blk_size  = BCDEC_BC3_BLOCK_SIZE;
        break;
    case comp_flags::BC7:
        transform = bcdec_bc7;
        blk_size  = BCDEC_BC7_BLOCK_SIZE;
        break;
    default:
        break;
    }

    if(!transform)
        Throw(texture_decode_not_implemented(
            "this variation of BCn not implemented"));

    auto blk_pitch = size.w / 4;
    for(auto y : stl_types::range<int>(size.h / 4))
        for(auto x : stl_types::range<int>(blk_pitch))
        {
            auto blk_idx = (y * blk_pitch + x) * blk_size;
            transform(
                data.subspan(blk_idx, blk_size).data(),
                &out.at((y * size.w + x) * 16),
                out_pitch);
        }

    Coffee::cDebug(
        "Expanded BCn image from {0} bytes to {1} bytes",
        data.size_bytes(),
        out.size());
    return out;
}
#endif

#if defined(GLEAM_ENABLE_SOFTWARE_PVRTC)
static std::vector<char> software_decode_pvrtc_etc1(
    PixDesc const&               fmt,
    semantic::Span<const char>&& data,
    size_3d<i32> const&          size)
{
    using typing::pixels::comp_flags;
    using enum_helpers::feval;
    std::vector<char> out;
    out.resize(size.w * size.h * 4);
    if(fmt.pixfmt == pix_fmt::PVRTC)
        pvr::PVRTDecompressPVRTC(
            data.data(),
            feval(fmt.cmpflg, comp_flags::PVRTC_BPP2) ? 1 : 0,
            size.w,
            size.h,
            reinterpret_cast<uint8_t*>(out.data()));
    else if(fmt.pixfmt == pix_fmt::ETC1)
        pvr::PVRTDecompressETC(data.data(), size.w, size.h, out.data(), 0);
    return out;
}
#endif

#if defined(GLEAM_ENABLE_SOFTWARE_BCN) || \
    defined(GLEAM_ENABLE_SOFTWARE_PVRTC) || defined(COFFEE_EMSCRIPTEN)
std::future<std::vector<char>> texture_t::software_decode(
    semantic::Span<const char>&& data, size_3d<i32> size, i32 mipmap)
{
    using typing::pixels::comp_flags;

    if(!m_decoder_queue)
    {
        if(auto res = rq::runtime_queue::CreateNewThreadQueue(
               "gleam::Texture Decoder Queue");
           res.has_error())
            Throw(
                texture_decode_not_available("failed to spawn decoder queue"));
        else
            m_decoder_queue = res.value();
    }

    (void)mipmap;

#if defined(GLEAM_ENABLE_SOFTWARE_BCN)
    if(m_format.pixfmt == pix_fmt::BCn)
    {
        auto task = rq::dependent_task<void, std::vector<char>>::CreateSource(
            [fmt = m_format, data, size]() mutable {
                return software_decode_bcn(fmt, std::move(data), size);
            });
        auto fut = task->output.get_future();
        auto res = rq::runtime_queue::Queue(m_decoder_queue, std::move(task));
        if(res.has_error())
            Throw(rq::runtime_queue_error("failed to queue BCn decode"));
        return fut;
    }
#endif
#if defined(GLEAM_ENABLE_SOFTWARE_PVRTC)
    if(stl_types::any_of(m_format.pixfmt, pix_fmt::PVRTC, pix_fmt::ETC1))
    {
        auto task = rq::dependent_task<void, std::vector<char>>::CreateSource(
            [fmt = m_format, data, size]() mutable {
                return software_decode_pvrtc_etc1(fmt, std::move(data), size);
            });
        auto fut = task->output.get_future();
        auto res = rq::runtime_queue::Queue(m_decoder_queue, std::move(task));
        if(res.has_error())
            Throw(rq::runtime_queue_error("failed to queue PVRTC/ETC1 decode"));
        return fut;
    }
#endif
    if(m_workarounds.tex.requires_aligned)
    {
        // On Emscripten for example, loading RGB565 from addresses not aligned
        // with 2-bytes causes clown vomit
        if(m_format.pixfmt == pix_fmt::RGB565)
        {
            auto task =
                rq::dependent_task<void, std::vector<char>>::CreateSource(
                    [data]() mutable {
                        /* Source pointer may be at an odd byte offset in the
                         * WASM heap. Emscripten's glTexSubImage3D does ptr >>>
                         * 1 to index HEAPU16, truncating the low bit and
                         * swapping bytes within every u16. Copying to a heap
                         * vector guarantees 2-byte alignment so the index is
                         * correct. */
                        std::vector<char> out(data.begin(), data.end());
                        fprintf(
                            stderr,
                            "Alignment: %li",
                            reinterpret_cast<intptr_t>(out.data()) & 0x1);
                        return out;
                    });
            auto fut = task->output.get_future();
            auto res =
                rq::runtime_queue::Queue(m_decoder_queue, std::move(task));
            if(res.has_error())
                Throw(rq::runtime_queue_error("failed to queue RGB565 decode"));
            return fut;
        }
    }

    Throw(texture_decode_not_implemented(
        "attempted doing software decode, but no implementation found"));
}
#endif

} // namespace gleam
