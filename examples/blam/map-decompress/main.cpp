#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_versions.h>
#include <coffee/application/application_start.h>
#include <coffee/core/coffee_args.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <cxxopts.hpp>
#include <magic_enum/magic_enum.hpp>
#include <url/url.h>

#include <filesystem>
#include <fstream>

using Coffee::Logging::cDebug;
using Coffee::Logging::cWarning;

namespace {

/* Inflates a Halo CE Xbox map. The output keeps the original header, and
 * decomp_len then matches the file size, which loaders take as uncompressed. */
bool decompress_map(
    std::filesystem::path const& input, std::filesystem::path const& output)
{
    using Ver = blam::xbox_version_t;

    Coffee::Resource map_file(
        platform::url::constructors::MkUrl(input.string()));
    if(!Coffee::FileMap(map_file))
    {
        cWarning("Could not open map file: {}", input.string());
        return false;
    }
    semantic::BytesConst const bytes = map_file;
    auto map_ = blam::map_container<Ver>::from_bytes(bytes, Ver());
    if(map_.has_error())
    {
        cWarning(
            "{}: failed to open map: {}",
            input.string(),
            magic_enum::enum_name(map_.error()));
        return false;
    }
    auto const& map = map_.value();

    if(map.decompressed.empty())
    {
        cWarning("{}: already uncompressed, skipping", input.string());
        return true;
    }

    auto expected = blam::from_le(map.map->decomp_len);
    if(map.decompressed.size() != expected)
        cWarning(
            "{}: decompressed {} bytes, header says {}",
            input.string(),
            map.decompressed.size(),
            expected);

    std::ofstream out(output, std::ios::binary);
    out.write(
        map.decompressed.data(),
        static_cast<std::streamsize>(map.decompressed.size()));
    if(!out.good())
    {
        cWarning("Failed to write {}", output.string());
        return false;
    }

    cDebug(
        "{} ({}): {} -> {} bytes, wrote {}",
        input.string(),
        map.name(),
        bytes.size,
        map.decompressed.size(),
        output.string());
    return true;
}

} // namespace

int decompress_main()
{
    cxxopts::Options options(
        "Blam Map Decompressor",
        "Inflates compressed Halo CE Xbox .map files");
    options.add_options("Help")("h,help", "Show this help info");
    options.positional_help(
        "[input.map] [output.map] | [input.map...] [output dir]");

    auto& args      = Coffee::GetInitArgs();
    auto  arguments = options.parse(args.size(), args.data());
    if(Coffee::BaseArgParser::PerformDefaults(options, args) >= 0)
        return 0;

    auto const& paths = arguments.unmatched();
    if(paths.size() < 2)
    {
        cWarning("Needs an input and an output, check --help");
        std::quick_exit(1);
    }

    std::filesystem::path target(paths.back());
    bool const            to_dir =
        paths.size() > 2 || std::filesystem::is_directory(target);
    if(to_dir)
    {
        std::error_code ec;
        std::filesystem::create_directories(target, ec);
        if(ec)
        {
            cWarning("Could not create {}: {}", target.string(), ec.message());
            std::quick_exit(1);
        }
    }

    bool ok = true;
    for(size_t i = 0; i < paths.size() - 1; ++i)
    {
        std::filesystem::path input(paths[i]);
        auto output = to_dir ? target / input.filename() : target;
        if(std::filesystem::exists(output) &&
           std::filesystem::equivalent(input, output))
        {
            cWarning("{}: refusing to overwrite the input", input.string());
            ok = false;
            continue;
        }
        ok &= decompress_map(input, output);
    }

    std::quick_exit(ok ? 0 : 1);
}

COFFEE_APPLICATION_MAIN_CUSTOM(decompress_main, 0x1 | 0x2)
