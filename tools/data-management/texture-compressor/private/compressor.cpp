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
    std::string const&      source_file,
    u32                     resolution,
    std::string_view const& extension)
{
    auto base_name
        = platform::url::Path(source_file).fileBasename().removeExt().internUrl;
    return fmt::format("{}.{}.{}", source_file, resolution, extension);
}

} // namespace

bool etc2_compress(
    std::string const&      file,
    std::vector<u32> const& resolutions,
    typing::PixCmp          format)
{
    compressor::rgbaf_image_t imagef;
    {
        Coffee::stb::stb_error img_err;
        if(!compressor::LoadData(
               &imagef,
               Coffee::Resource(platform::url::constructors::MkSysUrl(file)),
               img_err,
               format))
        {
            cBasicPrint("Failed to decode image to rgba32f");
            return false;
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
        return false;
    }

    //        auto out_name = replace::str<char>(out_format, "{resolution}",
    //        "all"); out_name      = replace::str<char>(out_name, "{codec}",
    //        "etc2");
    auto out_name = create_output_name(file, 0, "etc2");

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
            auto pixcmp
                = format == "rgba" ? typing::PixCmp::RGBA : typing::PixCmp::RGB;

            if(codec == "etc2")
                etc2_compress(file, resolutions, pixcmp);
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(cooker_main, 0x1 | 0x2)
