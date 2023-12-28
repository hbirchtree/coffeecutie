#include <coffee/application/application_start.h>
#include <fmt_extensions/url_types.h>
#include <peripherals/stl/string_ops.h>

#include <cxxopts.hpp>
#include <glad/gl.h>
#include <ktx.h>
#include <magic_enum.hpp>

#include "decoder.h"
#include "encoder.h"

#include <coffee/core/debug/formatting.h>

using namespace Coffee::Logging;
using libc_types::i32;
using libc_types::u32;
using libc_types::u8;
using platform::url::constructors::MkSysUrl;

namespace {

const fmt::format_string<std::string, u32, std::string> out_format = "{}.{}.{}";

inline std::string create_output_name(
    platform::url::Path const& base_dir,
    std::string const&         source_file,
    u32                        resolution,
    std::string_view const&    extension)
{
    auto base_name =
        (base_dir / platform::url::Path(source_file).fileBasename().removeExt())
            .internUrl;
    return fmt::format("{}.{}.{}", base_name, resolution, extension);
}

std::map<std::string, compressor::rgba_image_t> image_cache;

inline compressor::rgba_image_t& cache_image(std::string filename)
{
    if(auto it = image_cache.find(filename); it != image_cache.end())
    {
        cBasicPrint("Cache hit for {}", filename);
        return it->second;
    }
    Coffee::stb::stb_error img_err;
    if(!compressor::LoadData(
           &image_cache[filename],
           Coffee::Resource(MkSysUrl(filename)),
           img_err,
           typing::PixCmp::RGBA))
    {
        cBasicPrint("Failed to decode image to rgba8");
        return image_cache[filename];
    }
    cBasicPrint("Caching decoded image for {}", filename);
    return image_cache[filename];
}

bool save_ktx_to_file(std::string const& out_name, ktxTexture1* texture)
{
    auto out_stream = fopen(out_name.c_str(), "wb+");
    if(!out_stream)
    {
        cBasicPrint("Failed to open for writing: {0}", out_name);
        return false;
    }
    auto error = ktxTexture_WriteToStdioStream(ktxTexture(texture), out_stream);
    if(error != ktx_error_code_e::KTX_SUCCESS)
        cBasicPrint("Error writing KTX: {0}", magic_enum::enum_name(error));
    fclose(out_stream);
    return true;
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
               Coffee::Resource(MkSysUrl(file)),
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
    return save_ktx_to_file(
        create_output_name(base_dir, file, 0, "etc2"), *res);
}

bool bcn_compress(
    platform::url::Path const&               base_dir,
    std::string const&                       file,
    [[maybe_unused]] std::vector<u32> const& resolutions,
    [[maybe_unused]] std::string const&      codec,
    typing::PixCmp                           format,
    std::string const&                       channels,
    [[maybe_unused]] quality_mode            quality)
{
    using typing::pixels::CompFlags;

    compressor::rgba_image_t image = cache_image(file).copy();

    if(format != typing::PixCmp::RGBA)
    {
        auto remapped = compressor::map_channels(image, channels);
        if(!remapped.has_value())
            return false;
        image = std::move(remapped.value());
    }

    auto out_name = create_output_name(base_dir, file, 0, "png");

    CompFlags bcn_format = CompFlags::BC1;
    if(codec == "bc2" || codec == "bc3")
        bcn_format = CompFlags::BC3;
    else if(codec == "bc4")
        bcn_format = CompFlags::BC4;
    else if(codec == "bc5")
        bcn_format = CompFlags::BC5;
    else if(codec == "bc6")
        bcn_format = CompFlags::BC6H;
    else if(codec == "bc7")
        bcn_format = CompFlags::BC7;

    auto res = compressor::bcn::encode(image, bcn_format, format);

    if(!res)
    {
        cBasicPrint("Encoding {} to {} failed", file, codec);
        return false;
    }

    return save_ktx_to_file(
        create_output_name(base_dir, file, 0, codec), *res);
}

bool png_compress(
    platform::url::Path const& base_dir,
    std::string const&         file,
    std::vector<u32> const&    resolutions,
    typing::PixCmp             format,
    std::string const&         channels,
    quality_mode               quality)
{
    compressor::rgba_image_t image = cache_image(file).copy();

    if(format != typing::PixCmp::RGBA)
    {
        auto remapped = compressor::map_channels(image, channels);
        if(!remapped.has_value())
            return false;
        image = std::move(remapped.value());
    }

    Coffee::Resource       out(platform::url::constructors::MkSysUrl(
        create_output_name(base_dir, file, 0, "png")));
    Coffee::stb::stb_error img_ec;
    auto                   png = Coffee::PNG::Save(image, img_ec);
    out                        = png;
    if(img_ec)
        cBasicPrint("Failed to encode PNG");
    if(!Coffee::FileCommit(
           out,
           semantic::RSCA::NewFile | semantic::RSCA::WriteOnly |
               semantic::RSCA::Discard))
        cBasicPrint(
            "Failed to save PNG file, {} bytes, {}x{} {} channels",
            png.size,
            image.size.w,
            image.size.h,
            image.bpp);

    return true;
}

bool raw_include(
    platform::url::Path const& base_dir,
    std::string const&         file,
    std::vector<u32> const&    resolutions,
    typing::PixCmp             format,
    std::string const&         channels)
{
    compressor::rgba_image_t image = cache_image(file).copy();

    if(format != typing::PixCmp::RGBA)
    {
        auto remapped = compressor::map_channels(image, channels);
        if(!remapped.has_value())
            return false;
        image = std::move(remapped.value());
    }

    ktx_uint32_t ktx_format = GL_RGBA8;
    switch(format)
    {
    case typing::PixCmp::R:
        ktx_format = GL_R8;
        break;
    case typing::PixCmp::RG:
        ktx_format = GL_RG8;
        break;
    case typing::PixCmp::RGB:
        ktx_format = GL_RGB8;
        break;
    default:
        break;
    }

    ktxTextureCreateInfo info = {
        .glInternalformat = ktx_format,
        .vkFormat         = 0,
        .pDfd             = nullptr,
        .baseWidth        = image.size.w,
        .baseHeight       = image.size.h,
        .baseDepth        = 1,
        .numDimensions    = 2,
        .numLevels        = 1,
        .numLayers        = 1,
        .numFaces         = 1,
        .isArray          = KTX_FALSE,
        .generateMipmaps  = resolutions.size() > 1 ? KTX_TRUE : KTX_FALSE,
    };

    ktxTexture1* texture{};
    auto         error =
        ktxTexture1_Create(&info, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture);

    if(error != ktx_error_code_e::KTX_SUCCESS)
    {
        cBasicPrint("Failed to create KTX: {}", magic_enum::enum_name(error));
        return false;
    }

    error = ktxTexture_SetImageFromMemory(
        ktxTexture(texture),
        0,
        0,
        0,
        image.data,
        image.size.area() * image.bpp);
    if(error != ktx_error_code_e::KTX_SUCCESS)
    {
        cBasicPrint("Failed to create KTX: {}", magic_enum::enum_name(error));
        return false;
    }

    auto out_name = create_output_name(base_dir, file, 0, "raw");
    error = ktxTexture_WriteToNamedFile(ktxTexture(texture), out_name.c_str());
    if(error != ktx_error_code_e::KTX_SUCCESS)
    {
        cBasicPrint(
            "Failed to write KTX to file: {}", magic_enum::enum_name(error));
        return false;
    }
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
            else if(codec.starts_with("bc"))
                bcn_compress(
                    base_dir,
                    file,
                    resolutions,
                    codec,
                    pixcmp,
                    format,
                    release_quality);
            else if(codec == "png")
                png_compress(
                    base_dir,
                    file,
                    resolutions,
                    pixcmp,
                    format,
                    release_quality);
            else if(codec == "raw")
                raw_include(base_dir, file, resolutions, pixcmp, format);
        }

        image_cache.clear();
    }

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(cooker_main, 0x1 | 0x2)
