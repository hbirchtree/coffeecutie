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
 * Xbox maps carry their pixels themselves and are written out inflated, with
 * swizzled textures made linear.
 *
 * Usage: MapTranscode <in.map> [in bitmaps.map] --output-map <out.map>
 *        [--output-bitmaps <out bitmaps.map>] */

#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/argument_handling.h>
#include <coffee/core/coffee.h>

#include <blam/transcode/map_transcode.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_versions.h>
#include <cxxopts.hpp>
#include <filesystem>

#include "cfiles.h"
#include "coffee/application/application_start.h"
#include "coffee/core/coffee_args.h"
#include "coffee/core/url.h"

#include <fstream>
#include <optional>
#include <vector>

using namespace Coffee;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

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

i32 coffee_main(i32, cstring_w*)
{
    cxxopts::ParseResult arguments;
    {
        cxxopts::Options options(
            "MapTranscoder", "A Blam! map bitmap transcoder");
        Coffee::BaseArgParser::GetBase(options);
        options.custom_help(
            "[input map.map] [input bitmaps.map, not for Xbox] [OPTION...]");

        options.add_options("Target")
            //
            ("target",
             "Target device, determines default texture formats; Gekko, "
             "PowerVR, ES2, ES3, or none to only deswizzle Xbox textures",
             cxxopts::value<std::string>()->default_value("Gekko"))
            //
            ("jobs",
             "Threads to encode images on",
             cxxopts::value<unsigned>()->default_value("1"))
            //
            ("etc-effort",
             "etc2comp effort for ES2/ES3, 0-100",
             cxxopts::value<float>()->default_value("40"))
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
             "Output patched bitmaps.map file",
             cxxopts::value<std::string>())
            //
            ("output-map",
             "Output patched map file, decompressed for Xbox",
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
                    " * Targets PowerVR SGX 5-series GPUs running OpenGL ES "
                    "2.0\n"
                    " * Keeps most formats that are compatible with baseline "
                    "OpenGL ES 2.0\n"
                    " * Transcodes BC1, RGB565 to PVRTCv1 RGB\n"
                    " * Transcodes BC2, BC3, RGBA8, XRGB8 to PVRTCv1 RGBA\n"
                    "\n"
                    " Adreno2xx (unimplemented):\n"
                    " * Targets Adreno 2xx GPUs running OpenGL ES 2.0\n"
                    " * Keeps most formats that are compatible with baseline "
                    "OpenGL ES 2.0\n"
                    " * Transcodes BC1, RGB565 to ETC1\n"
                    " * Transcodes BC2, BC3, RGBA8, XRGB8 to ATC RGBA\n"
                    "\n"
                    " ES2:\n"
                    " * Targets OpenGL ES 2.0 systems (non-PowerVR, e.g. "
                    "Mali-400MP)\n"
                    " * Keeps most formats that are compatible with baseline "
                    "OpenGL ES 2.0\n"
                    " * Transcodes BC1 to ETC1\n"
                    " * Transcodes BC2, BC3 to split-alpha ETC1 (RGB ETC1 + "
                    "alpha-as-luminance ETC1,\n"
                    "   combined in shader; needs special handling, matches "
                    "BC2/BC3 footprint)\n"
                    "\n"
                    " ES3:\n"
                    " * Targets OpenGL ES 3.0 systems\n"
                    " * Keeps most formats that are compatible with baseline "
                    "OpenGL ES 3.0\n"
                    " * Transcodes BC1 to ETC2 RGB\n"
                    " * Transcodes BC2, BC3 to ETC2 RGBA\n");
            }
            return 0;
        }

        if(arguments.unmatched().empty())
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

    bool const has_bitmaps = arguments.unmatched().size() > 1;
    if(!arguments.count("output-map") ||
       (has_bitmaps && !arguments.count("output-bitmaps")))
    {
        cWarning("requires --output-map, and --output-bitmaps with bitmaps.map");
        return 6;
    }

    Resource                map(MkUrl(arguments.unmatched()[0]));
    std::optional<Resource> bitmaps;
    if(has_bitmaps)
        bitmaps.emplace(MkUrl(arguments.unmatched()[1]));
    if(!FileMap(map, RSCA::ReadOnly) ||
       (bitmaps && !FileMap(*bitmaps, RSCA::ReadOnly)))
    {
        cWarning("Failed to open map/bitmaps file");
        return 3;
    }

    gsl::span<u8 const> map_span(
        reinterpret_cast<u8 const*>(map.data().data()), map.data().size());
    gsl::span<u8 const> bitmaps_span;
    if(bitmaps)
        bitmaps_span = gsl::span<u8 const>(
            reinterpret_cast<u8 const*>(bitmaps->data().data()),
            bitmaps->data().size());

    // Xbox maps are compressed on disc, they are transcoded inflated
    std::vector<char> inflated;
    {
        using Ver = blam::xbox_version_t;
        auto xbox = blam::map_container<Ver>::from_bytes(
            semantic::BytesConst::ofBytes(map_span.data(), map_span.size()),
            Ver());
        if(xbox.has_value() && !xbox.value().decompressed.empty())
        {
            inflated = std::move(xbox.value().decompressed);
            map_span = gsl::span<u8 const>(
                reinterpret_cast<u8 const*>(inflated.data()), inflated.size());
        }
    }

    mtx::kernel_fn kernel;
    if(target != "none")
    {
        auto found = mtx::make_kernel(
            target, lightmap_format, arguments["etc-effort"].as<float>());
        if(!found)
        {
            cWarning("No target defined, no transcode kernel");
            return 7;
        }
        kernel = std::move(*found);
    }

    std::vector<u8> out_map(map_span.begin(), map_span.end());
    std::vector<u8> out_bitmaps(bitmaps_span.begin(), bitmaps_span.end());

    auto res =
        mtx::transcode_map(
            map_span,
            bitmaps_span,
            out_map,
            out_bitmaps,
            kernel,
            {.threads = arguments["jobs"].as<unsigned>()});
    if(!res.ok)
    {
        cWarning("map parse failed: {0}", res.error);
        return 4;
    }
    auto const& st = res.stats;

    cDebug(
        "converted={0} deswizzled={1} skipped={2} read={3} saved={4} KiB",
        st.converted,
        st.deswizzled,
        st.skipped,
        st.skipped_read,
        st.saved_bytes / 1024);

    auto const out_map_path = arguments["output-map"].as<std::string>();
    if(!write_file(out_map_path.c_str(), out_map))
    {
        cWarning("failed to write output (or an output file already exists)");
        return 5;
    }
    cDebug("wrote {0}", out_map_path);
    if(has_bitmaps)
    {
        auto const out_bmap_path =
            arguments["output-bitmaps"].as<std::string>();
        if(!write_file(out_bmap_path.c_str(), out_bitmaps))
        {
            cWarning(
                "failed to write output (or an output file already exists)");
            return 5;
        }
        cDebug("wrote {0}", out_bmap_path);
    }
    return 0;
}

// Silent + custom arg handler
COFFEE_APPLICATION_MAIN_CUSTOM(coffee_main, 0x1 | 0x2)
