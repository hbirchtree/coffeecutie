#include <coffee/application/application_start.h>
#include <fmt_extensions/url_types.h>
#include <peripherals/stl/string_ops.h>

#include <cxxopts.hpp>
#include <ktx.h>
#include <magic_enum.hpp>

#include "decoder.h"
#include "encoder.h"

#include <coffee/core/debug/formatting.h>

using namespace Coffee::Logging;
using libc_types::i32;
using libc_types::u32;
using libc_types::u8;

namespace {

constexpr fmt::format_string<std::string, u32, std::string> out_format
    = "{}.{}.{}";

inline std::string create_output_name(
    platform::url::Path const& base_dir,
    std::string const&         source_file,
    u32                        resolution,
    std::string_view const&    extension)
{
    auto base_name
        = (base_dir
           / platform::url::Path(source_file).fileBasename().removeExt())
              .internUrl;
    return fmt::format("{}.{}.{}", base_name, resolution, extension);
}

} // namespace

enum class quality_mode
{
    fast_mode,
    release_mode,
};

bool etc2_compress(
    platform::url::Path const& base_dir,
    std::string const&         file,
    std::vector<u32> const&    resolutions,
    typing::PixCmp             format,
    std::string const&         channels,
    quality_mode               quality)
{
    compressor::rgbaf_image_t imagef;
    {
        Coffee::stb::stb_error img_err;
        if(!compressor::LoadData(
               &imagef,
               Coffee::Resource(platform::url::constructors::MkSysUrl(file)),
               img_err,
               typing::PixCmp::RGBA))
        {
            cBasicPrint("Failed to decode image to rgba32f");
            return false;
        }
    }

    if(format != typing::PixCmp::RGBA)
    {
        auto remapped = compressor::map_channels(imagef, channels);
        if(!remapped.has_value())
            return false;
        imagef = std::move(remapped.value());
    }

    auto out_fmt = compressor::etc2::format_t::RGBA8;

    if(format == typing::PixCmp::R)
        out_fmt = compressor::etc2::format_t::R11;
    if(format == typing::PixCmp::RG)
        out_fmt = compressor::etc2::format_t::RG11;
    if(format == typing::PixCmp::RGB)
        out_fmt = compressor::etc2::format_t::RGB8;

    auto res = compressor::etc2::encode(
        imagef,
        out_fmt,
        compressor::settings_t{
            .quality = quality == quality_mode::release_mode ? 1.f : 0.1f,
            .mipmaps = static_cast<uint32_t>(resolutions.size()),
        });

    if(!res)
    {
        cBasicPrint("Failed to encode image {0}", file);
        return false;
    }
    auto out_name   = create_output_name(base_dir, file, 0, "etc2");
    auto out_stream = fopen(out_name.c_str(), "wb+");
    if(!out_stream)
    {
        cBasicPrint("Failed to open for writing: {0}", out_name);
        return false;
    }
    auto error = ktxTexture_WriteToStdioStream(ktxTexture(*res), out_stream);
    if(error != ktx_error_code_e::KTX_SUCCESS)
        cBasicPrint("Error writing KTX: {0}", magic_enum::enum_name(error));
    fclose(out_stream);
    return true;
}

i32 cooker_main(i32 argc, char** argv)
{
    cxxopts::Options opts(
        "TextureCompressor", "All-purpose texture compressor");
    opts.positional_help("input files")
        .add_options()
        //
        ("c,codec",
         "Codec with pixel format to output in the format [codec]:[format], "
         "multiple can be specified",
         cxxopts::value<std::string>())
        //
        ("r,resolution",
         "Resolution to output, multiple can be specified",
         cxxopts::value<std::string>())
        //
        ("punchthrough-color",
         "Punchthrough color when putting RGBA images in RGB formats (eg. BC1 "
         "in RGB mode or ETC1)",
         cxxopts::value<std::string>())
        //
        ("mode",
         "Either \"fast\" or \"release\" to vary the output quality of "
         "encoders",
         cxxopts::value<std::string>())
        //
        ("o,output",
         "Output directory, files are automatically named by resolution and "
         "codec",
         cxxopts::value<std::string>());

    auto res = opts.parse(argc, argv);

    if(res.unmatched().size() < 1)
    {
        cFatal("No files provided");
        return 1;
    }

    std::vector<u32>                                 resolutions;
    std::vector<std::pair<std::string, std::string>> codecs;
    platform::url::Path                              base_dir;
    quality_mode release_quality    = quality_mode::fast_mode;
    std::string  punchthrough_color = "0xFF00FF";

    for(cxxopts::KeyValue const& arg : res.arguments())
    {
        if(arg.key() == "resolution")
            resolutions.push_back(std::stoi(arg.value()));
        else if(arg.key() == "codec")
        {
            using namespace stl_types::str;
            std::string codec, format;
            for(auto part : split::str<char>(arg.value().c_str(), ':'))
            {
                if(codec.empty())
                    codec = part;
                else
                    format = part;
            }
            codecs.push_back(std::make_pair(codec, format));
        } else if(arg.key() == "punchthrough-color")
            punchthrough_color = arg.value();
        else if(arg.key() == "output")
            base_dir = platform::url::Path(arg.value());
        else if(arg.key() == "mode" && arg.value() == "release")
            release_quality = quality_mode::release_mode;
    }

    if(resolutions.empty() || codecs.empty())
    {
        cFatal("No resolutions or codecs specified");
        return 1;
    }

    for(auto const& file : res.unmatched())
    {
        cBasicPrint("Processing {0} -> {1} x {2}", file, codecs, resolutions);

        for(auto [codec, format] : codecs)
        {
            auto pixcmp = format == "rgba"           ? typing::PixCmp::RGBA
                          : format == "rgb"          ? typing::PixCmp::RGB
                          : (format == "rg" || "ra") ? typing::PixCmp::RG
                                                     : typing::PixCmp::R;

            if(codec == "etc2")
                etc2_compress(
                    base_dir,
                    file,
                    resolutions,
                    pixcmp,
                    format,
                    release_quality);
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(cooker_main, 0x1 | 0x2)
