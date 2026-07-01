/* Offline Halo map bitmap transcoder.
 *
 * Rewrites a PC map's bitmaps into GameCube GX-native tiled layouts (CMPR /
 * RGB565 / I8 / IA8) so the console uploads them directly with no runtime
 * decode. Pixel data is patched in place at the same offset in a copy of the
 * shared bitmaps.map (every GX target is <= its source in bytes), and each
 * transcoded image's `format`/`size` fields are updated in a copy of the level
 * map. Images that don't fit their slot, or in a format we don't handle, are
 * left untouched (the runtime still decodes those the old way).
 *
 * Usage: MapTranscode <in.map> <in bitmaps.map> <out.map> <out bitmaps.map> */

#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/argument_handling.h>
#include <coffee/core/coffee.h>

#include <peripherals/semantic/chunk.h>

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_endian.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_tag_classes.h>

#include "transcode.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>

using namespace Coffee;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;
using version_t = blam::pc_version_t;

#define LOG(...)               \
    do                         \
    {                          \
        std::printf(__VA_ARGS__); \
        std::printf("\n");     \
        std::fflush(stdout);   \
    } while(0)

static std::vector<u8> read_file(char const* path)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if(!f)
        return {};
    std::streamsize const n = f.tellg();
    f.seekg(0);
    std::vector<u8> out(static_cast<size_t>(n));
    if(n > 0)
        f.read(reinterpret_cast<char*>(out.data()), n);
    return out;
}

static bool write_file(char const* path, std::vector<u8> const& data)
{
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if(!f)
        return false;
    f.write(reinterpret_cast<char const*>(data.data()), data.size());
    return f.good();
}

struct stats
{
    u32 converted{0}, skipped_fmt{0}, skipped_fit{0}, skipped_read{0};
    u32 saved_bytes{0};
};

static void patch_u16(std::vector<u8>& buf, size_t off, u16 v)
{
    std::memcpy(buf.data() + off, &v, sizeof(v)); // host LE == map LE
}
static void patch_u32(std::vector<u8>& buf, size_t off, u32 v)
{
    std::memcpy(buf.data() + off, &v, sizeof(v));
}

i32 coffee_main(i32, cstring_w*)
{
    using blam::from_le;
    using blam::bitm::image_t;

    // Collect positional args (GetInitArgs()[0] is argv0; skip it + any flags).
    // NB: the Coffee framework parses argv with cxxopts and rejects unknown
    // --options before main runs, so the lightmap-CMPR switch is an env var.
    std::vector<char const*> pos;
    auto const&              args = GetInitArgs();
    for(size_t i = 1; i < args.size(); i++)
        if(args[i] && args[i][0] != '-')
            pos.push_back(args[i]);

    // LM_CMPR=1: compress uncompressed colour (lightmaps) to CMPR (4bpp vs 16bpp).
    char const* lm_env      = std::getenv("LM_CMPR");
    bool const  rgb565_cmpr = lm_env && lm_env[0] && lm_env[0] != '0';
    if(pos.size() < 4)
    {
        LOG("usage: MapTranscode <in.map> <in bitmaps.map> <out.map> <out "
            "bitmaps.map>");
        return 2;
    }
    char const* in_map     = pos[0];
    char const* in_bitmaps = pos[1];
    char const* out_map_p  = pos[2];
    char const* out_bmap_p = pos[3];
    LOG("in map=%s", in_map);
    LOG("in bitmaps=%s", in_bitmaps);

    std::vector<u8> map_bytes = read_file(in_map);
    std::vector<u8> bitmaps   = read_file(in_bitmaps);
    if(map_bytes.empty() || bitmaps.empty())
    {
        LOG("failed to read input (map=%zu bitmaps=%zu bytes)",
            map_bytes.size(),
            bitmaps.size());
        return 3;
    }
    LOG("map=%zu KiB, bitmaps=%zu KiB",
        map_bytes.size() / 1024,
        bitmaps.size() / 1024);

    std::vector<u8> out_map     = map_bytes; // patched formats/sizes
    std::vector<u8> out_bitmaps = bitmaps;   // patched pixels

    auto parsed = blam::map_container<version_t>::from_bytes(
        semantic::BytesConst::ofBytes(map_bytes.data(), map_bytes.size()),
        version_t{},
        [](std::string_view, libc_types::i16) {});
    if(parsed.has_error())
    {
        LOG("map parse failed err=%d", static_cast<int>(parsed.error()));
        return 4;
    }
    auto const& c   = parsed.value();
    u8 const*   base = map_bytes.data();

    stats     st;
    u32 const tag_count = from_le(c.tags->tag_count);
    for(u32 i = 0; i < tag_count; i++)
    {
        blam::tag_t const& t = c.tags->tags(c.map)[i];
        if(!t.matches(blam::tag_class_t::bitm))
            continue;
        auto hr = t.data<blam::bitm::header_t>(c.magic);
        if(hr.has_error())
            continue;
        auto imgs = hr.value()->images.data(c.magic);
        if(imgs.has_error())
            continue;

        for(image_t const& img : imgs.value())
        {
            auto const fmt = from_le(img.format);
            i16 const  w   = from_le(img.isize.x);
            i16 const  h   = from_le(img.isize.y);
            if(w <= 0 || h <= 0)
                continue;

            auto tgt = mtx::target_for(fmt);
            if(!tgt)
            {
                st.skipped_fmt++;
                continue;
            }
            u32 const src_size = from_le(img.size);

            // Locate source pixels + their destination in the output copies.
            u8 const* src_px = nullptr;
            u8*       dst_px = nullptr;
            if(img.shared())
            {
                u32 const off = from_le(img.offset);
                if(static_cast<size_t>(off) + src_size > bitmaps.size())
                {
                    st.skipped_read++;
                    continue;
                }
                src_px = bitmaps.data() + off;
                dst_px = out_bitmaps.data() + off;
            } else
            {
                auto pix = blam::reference<u8>{
                    .count = img.size, .offset = img.offset}
                               .data(c.magic);
                if(pix.has_error())
                {
                    st.skipped_read++;
                    continue;
                }
                size_t const foff =
                    reinterpret_cast<u8 const*>(pix.value().data()) - base;
                src_px = map_bytes.data() + foff;
                dst_px = out_map.data() + foff;
            }

            std::vector<u8> tiled;
            mtx::gx_format  out_fmt;
            u32             levels = 0;
            if(!mtx::transcode_mipped(
                   fmt,
                   src_px,
                   src_size,
                   static_cast<u16>(w),
                   static_cast<u16>(h),
                   from_le(img.mipmaps),
                   rgb565_cmpr,
                   tiled,
                   out_fmt,
                   levels))
            {
                st.skipped_fit++;
                continue;
            }

            std::memcpy(dst_px, tiled.data(), tiled.size());

            // Patch the image_t in the output map: new format + size + GX maxlod
            // (levels-1). Member-address arithmetic (not offsetof) avoids
            // -Winvalid-offsetof on image_t's vector members.
            auto foff_of = [&](void const* p) -> size_t {
                return reinterpret_cast<u8 const*>(p) - base;
            };
            patch_u16(
                out_map,
                foff_of(&img.format),
                static_cast<u16>(gexxo::native::to_blam(out_fmt)));
            patch_u32(
                out_map, foff_of(&img.size), static_cast<u32>(tiled.size()));
            patch_u16(
                out_map,
                foff_of(&img.mipmaps),
                static_cast<u16>(levels > 0 ? levels - 1 : 0));

            st.converted++;
            st.saved_bytes += src_size - static_cast<u32>(tiled.size());
        }
    }

    LOG("converted=%u skipped(fmt=%u fit=%u read=%u) saved=%u KiB",
        st.converted,
        st.skipped_fmt,
        st.skipped_fit,
        st.skipped_read,
        st.saved_bytes / 1024);

    if(!write_file(out_map_p, out_map) || !write_file(out_bmap_p, out_bitmaps))
    {
        LOG("failed to write output");
        return 5;
    }
    LOG("wrote %s + %s", out_map_p, out_bmap_p);
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
