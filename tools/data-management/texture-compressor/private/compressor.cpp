#include <coffee/application/application_start.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>
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
        ("o,output",
         "Output file format, {codec} and {resolution} can be substituted for "
         "their respective values",
         cxxopts::value<std::string>());

    auto res = opts.parse(argc, argv);

    if(res.unmatched().size() < 1)
    {
        cFatal("No files provided");
        return 1;
    }

    std::vector<u32>                                 resolutions;
    std::vector<std::pair<std::string, std::string>> codecs;
    std::string punchthrough_color = "0xFF00FF";
    std::string out_format = platform::url::Path(res.unmatched().front())
                                 .removeExt()
                                 .addExtension("{resolution}.{codec}")
                                 .internUrl;

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
            out_format = arg.value();
    }

    if(resolutions.empty() || codecs.empty())
    {
        cFatal("No resolutions or codecs specified");
    }

    using Coffee::Resource;
    using Coffee::stb::stb_error;
    using typing::PixCmp;
    using namespace platform::url::constructors;
    using namespace stl_types::str;

    for(auto const& file : res.unmatched())
    {
        cBasicPrint("Processing {0} -> {1} x {2}", file, codecs, resolutions);

        compressor::rgbaf_image_t imagef;
        {
            stb_error img_err;
            if(!compressor::LoadData(
                   &imagef, Resource(MkSysUrl(file)), img_err, PixCmp::RGBA))
            {
                cBasicPrint("Failed to decode image to rgba32f");
                return 1;
            }
        }
        auto res = compressor::etc2::encode(
            imagef,
            compressor::etc2::format_t::RGB8A1,
            compressor::settings_t{
                .quality = 1.f,
                .mipmaps = static_cast<uint32_t>(resolutions.size()),
            });

        if(!res)
        {
            cBasicPrint("Failed to encode image {0}", file);
            continue;
        }

        auto out_name = replace::str<char>(out_format, "{resolution}", "all");
        out_name      = replace::str<char>(out_name, "{codec}", "etc2");

        auto error
            = ktxTexture_WriteToNamedFile(ktxTexture(*res), out_name.c_str());
        if(error != ktx_error_code_e::KTX_SUCCESS)
            cBasicPrint("Error writing KTX: {0}", magic_enum::enum_name(error));
    }

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(cooker_main, 0x1 | 0x2)
