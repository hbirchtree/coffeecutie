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

#include <cxxopts.hpp>
#include <filesystem>
#include <magic_enum/magic_enum.hpp>
#include <peripherals/semantic/chunk.h>

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_endian.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_tag_classes.h>

#include "cfiles.h"
#include "coffee/application/application_start.h"
#include "coffee/core/coffee_args.h"
#include "coffee/core/url.h"
#include "transcode.h"

#include <cstring>
#include <fstream>
#include <vector>

using namespace Coffee;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;
using version_t = blam::pc_version_t;

static bool write_file(char const* path, std::vector<u8> const& data)
{
    if(std::filesystem::exists(path))
        return false;
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if(!f)
        return false;
    f.write(reinterpret_cast<char const*>(data.data()), data.size());
    return f.good();
}

struct stats
{
    u32 converted{0}, skipped{0}, skipped_read{0};
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

    cxxopts::ParseResult arguments;
    {
        cxxopts::Options options(
            "MapTranscoder",
            "A Blam! map bitmap transcoder");
        Coffee::BaseArgParser::GetBase(options);
        options.custom_help("[input map.map] [input bitmaps.map] [OPTION...]");

        options.add_options("Target")
            //
            ("target",
             "Target device, determines default texture formats; Gekko, PowerVR, ES2, ES3",
             cxxopts::value<std::string>()->default_value("Gekko"))
            //
            ;

        options.add_options("Texture formats")
            //
            ("lightmap-format",
             "Target format for lightmaps (RGB): native or cmpr (Gekko only)",
             cxxopts::value<std::string>())
            //
            ("rgb-format",
             "Target format for color RGB",
             cxxopts::value<std::string>())
            //
            ("rgba-format",
             "Target format for color RGBA",
             cxxopts::value<std::string>())
            //
            ;
        options.add_options("File I/O")
            //
            ("output-bitmaps",
             "Output patched bitmaps.map file (Gekko target)",
             cxxopts::value<std::string>())
            //
            ("output-map",
             "Output patched map file (Gekko target)",
             cxxopts::value<std::string>())
            //
            ;

        auto& args = GetInitArgs();
        arguments  = options.parse(args.size(), args.data());
        if(BaseArgParser::PerformDefaults(options, args) >= 0)
        {
            if(arguments.contains("help"))
            {
                cBasicPrint(
                    "\n"
                    " Target types:\n"
                    "\n"
                    " Gekko:\n"
                    " * Targets Gekko's Flipper GPU with the GX API\n"
                    " * Tiles BC1 to CMPR format\n"
                    " * Transcodes RGB565 lightmaps to CMPR\n"
                    " * Tiles A8, AY8, Y8, A8Y8, RGB565 in equivalent format\n"
                    " * Downgrades BC2/BC3 to CMPR\n"
                    " * Compresses RGB565 as CMPR\n"
                    "\n"
                    " PowerVR:\n"
                    " * Targets PowerVR SGX 5-series GPUs running OpenGL ES 2.0\n"
                    " * Keeps most formats that are compatible with baseline OpenGL ES 2.0\n"
                    " * Transcodes BC1, RGB565 to PVRTCv1 RGB\n"
                    " * Transcodes BC2, BC3, RGBA8, XRGB8 to PVRTCv1 RGBA\n"
                    "\n"
                    " Adreno2xx (unimplemented):\n"
                    " * Targets Adreno 2xx GPUs running OpenGL ES 2.0\n"
                    " * Keeps most formats that are compatible with baseline OpenGL ES 2.0\n"
                    " * Transcodes BC1, RGB565 to ETC1\n"
                    " * Transcodes BC2, BC3, RGBA8, XRGB8 to ATC RGBA\n"
                    "\n"
                    " ES2:\n"
                    " * Targets OpenGL ES 2.0 systems (non-PowerVR, e.g. Mali-400MP)\n"
                    " * Keeps most formats that are compatible with baseline OpenGL ES 2.0\n"
                    " * Transcodes BC1 to ETC1\n"
                    " * Transcodes BC2, BC3 to split-alpha ETC1 (RGB ETC1 + alpha-as-luminance ETC1,\n"
                    "   combined in shader; needs special handling, matches BC2/BC3 footprint)\n"
                    "\n"
                    " ES3:\n"
                    " * Targets OpenGL ES 3.0 systems\n"
                    " * Keeps most formats that are compatible with baseline OpenGL ES 3.0\n"
                    " * Transcodes BC1 to ETC2 RGB\n"
                    " * Transcodes BC2, BC3 to ETC2 RGBA\n");
            }
            return 0;
        }

        if(arguments.unmatched().size() < 2)
        {
            return 1;
        }
    }

    auto const target = arguments["target"].as<std::string>();
    // Raw --lightmap-format value; each target interprets it (or ignores it)
    // in its own make_kernel -- "cmpr" only means something to Gekko.
    std::optional<std::string> const lightmap_format =
        arguments.count("lightmap-format")
            ? std::optional(arguments["lightmap-format"].as<std::string>())
            : std::nullopt;

    if(!arguments.count("output-map") || !arguments.count("output-bitmaps"))
    {
        cWarning("target requires --output-map and --output-bitmaps");
        return 6;
    }

    Resource map(MkUrl(arguments.unmatched()[0]));
    Resource bitmaps(MkUrl(arguments.unmatched()[1]));
    if(!FileMap(map, RSCA::ReadOnly) || !FileMap(bitmaps, RSCA::ReadOnly))
    {
        cWarning("Failed to open map/bitmaps file");
        return 3;
    }

    gsl::span<u8 const> map_span(
        reinterpret_cast<u8 const*>(map.data().data()), map.data().size());
    gsl::span<u8 const> bitmaps_span(
        reinterpret_cast<u8 const*>(bitmaps.data().data()),
        bitmaps.data().size());
    u8 const* base = map_span.data();

    auto parsed = blam::map_container<version_t>::from_bytes(
        semantic::BytesConst::ofBytes(map_span.data(), map_span.size()),
        version_t{},
        [](std::string_view, libc_types::i16) {});
    if(parsed.has_error())
    {
        cWarning("map parse failed err={0}", magic_enum::enum_name(parsed.error()));
        return 4;
    }
    auto const& c = parsed.value();

    // Patched output copies (only written for the Gekko target).
    std::vector<u8> out_map(map_span.begin(), map_span.end());
    std::vector<u8> out_bitmaps(bitmaps_span.begin(), bitmaps_span.end());

    // The active target's whole encode step, behind one uniform interface
    // (see transcode.h's kernel_fn doc). Adding a target = writing one
    // make_kernel()-style factory in transcode.h and one branch here; the
    // rest of this function (I/O, the per-image loop, patching) is generic.
    mtx::kernel_fn kernel;
    if(target == "Gekko")
        kernel = mtx::gekko::make_kernel(lightmap_format);
    else if(target == "PowerVR")
        kernel = mtx::powervr::make_kernel();
    else if(target == "ES2")
        kernel = mtx::es2::make_kernel();
    else if(target == "ES3")
        kernel = mtx::es3::make_kernel();
    else
    {
        cWarning("No target defined, no transcode kernel");
        return 7;
    }

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

            u32 const src_size = from_le(img.size);

            // Locate source pixels (as a span into the read-only mapped
            // input) + their destination offset in the output copies.
            gsl::span<u8 const> src_px;
            u8*                 dst_base = nullptr;
            size_t              dst_off  = 0;
            if(img.shared())
            {
                u32 const off = from_le(img.offset);
                if(static_cast<size_t>(off) + src_size > bitmaps_span.size())
                {
                    st.skipped_read++;
                    continue;
                }
                src_px   = bitmaps_span.subspan(off, src_size);
                dst_base = out_bitmaps.data();
                dst_off  = off;
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
                src_px   = map_span.subspan(foff, src_size);
                dst_base = out_map.data();
                dst_off  = foff;
            }

            auto result = kernel(
                fmt,
                src_px,
                src_size,
                static_cast<u16>(w),
                static_cast<u16>(h),
                from_le(img.mipmaps));
            if(!result)
            {
                st.skipped++;
                continue;
            }

            std::memcpy(dst_base + dst_off, result->data.data(), result->data.size());

            // Patch the image_t in the output map: new format + size +
            // mip levels, verbatim from the kernel. Member-address
            // arithmetic (not offsetof) avoids -Winvalid-offsetof on
            // image_t's vector members.
            auto foff_of = [&](void const* p) -> size_t {
                return reinterpret_cast<u8 const*>(p) - base;
            };
            patch_u16(
                out_map,
                foff_of(&img.format),
                static_cast<u16>(result->format));
            patch_u32(
                out_map,
                foff_of(&img.size),
                static_cast<u32>(result->data.size()));
            patch_u16(out_map, foff_of(&img.mipmaps), result->maxlod);

            st.converted++;
            st.saved_bytes += src_size - static_cast<u32>(result->data.size());
        }
    }

    cDebug(
        "converted={0} skipped={1} read={2} saved={3} KiB",
        st.converted,
        st.skipped,
        st.skipped_read,
        st.saved_bytes / 1024);

    auto const out_map_path  = arguments["output-map"].as<std::string>();
    auto const out_bmap_path = arguments["output-bitmaps"].as<std::string>();
    if(!write_file(out_map_path.c_str(), out_map) ||
       !write_file(out_bmap_path.c_str(), out_bitmaps))
    {
        cWarning("failed to write output (or an output file already exists)");
        return 5;
    }
    cDebug("wrote {0} + {1}", out_map_path, out_bmap_path);
    return 0;
}

// Silent + custom arg handler
COFFEE_APPLICATION_MAIN_CUSTOM(coffee_main, 0x1 | 0x2)
