
#include "blam/volta/blam_scenario.h"
#include "blam/volta/hsc/bytecode_common_v12.h"
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_versions.h>
#include <coffee/application/application_start.h>
#include <coffee/core/coffee_args.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <cstdlib>
#include <cxxopts.hpp>
#include <magic_enum/magic_enum.hpp>
#include <peripherals/libc/types.h>
#include <url/url.h>

using libc_types::i32;
using Coffee::Logging::cWarning;

template<blam::is_game_version Ver>
void open_map(std::string path, std::string output)
{
    Coffee::Resource map_file(platform::url::constructors::MkUrl(path));
    if(!Coffee::FileMap(map_file))
    {
        cWarning("Could not open map file: {}", path);
        std::exit(1);
    }
    auto map_ = blam::map_container<Ver>::from_bytes(map_file, Ver());
    if(map_.has_error())
    {
        cWarning("Failed to open map: {}", magic_enum::enum_name(map_.error()));
        std::exit(1);
    }
    blam::map_container<Ver> map = std::move(map_.value());
    blam::scn::scenario<Ver> const* scenario = map.scenario().value();
    auto fp = fopen(output.c_str(), "w+");
    auto bytecode = scenario->bytecode(map.magic);
    for(blam::hsc::global const& global : scenario->script.globals.data(map.magic).value())
    {
        auto name = global.name.str();
        auto type = magic_enum::enum_name(global.type);
        fprintf(fp, ";;   %04i: glob %-32.*s type=%.*s\n",
            global.index,
            static_cast<int>(name.size()), name.data(),
            static_cast<int>(type.size()), type.data());
    }
    for(auto const& fun : scenario->function_table(map.magic))
    {
        auto name = fun.name.str();
        auto sched = magic_enum::enum_name(fun.schedule);
        auto type = magic_enum::enum_name(fun.type);
        fprintf(fp, ";; 0x%04x: func %-32.*s schedule=%.*s return_type=%.*s\n",
            fun.index,
            static_cast<int>(name.size()), name.data(),
            static_cast<int>(sched.size()), sched.data(),
            static_cast<int>(type.size()), type.data());
    }
    for(auto const& opcode : bytecode)
    {
        auto repr = blam::hsc::to_string(opcode);
        fprintf(fp, "%.*s | op=%04i\n",
            static_cast<int>(repr.size()), repr.data(),
            static_cast<int>(opcode.opcode));
    }
    fclose(fp);
}

int dumper_main()
{
    cxxopts::Options options(
        "Blam Script Dumper",
        "An application to dump Halo Script from a .map file");

    options.positional_help("[map file] [output file.hsd]");

    options.add_options("Map parsing")
        //
        ("halo-version", "Version of Halo: pc xbox custom trial", cxxopts::value<std::string>())
        //
        ;

    auto& args = Coffee::GetInitArgs();
    auto arguments = options.parse(args.size(), args.data());
    if(Coffee::BaseArgParser::PerformDefaults(options, args) >= 0)
        return 0;

    if(arguments.unmatched().size() != 2)
    {
        cWarning("Needs exactly two arguments, check --help");
        std::exit(1);
    }

    std::string selected_version =
        arguments.as_optional<std::string>("halo-version").value_or("pc");

    if(selected_version == "pc")
        open_map<blam::pc_version_t>(
            arguments.unmatched().at(0), arguments.unmatched().at(1));
    if(selected_version == "xbox")
        open_map<blam::xbox_version_t>(
            arguments.unmatched().at(0), arguments.unmatched().at(1));
    if(selected_version == "custom")
        open_map<blam::custom_version_t>(
            arguments.unmatched().at(0), arguments.unmatched().at(1));
    if(selected_version == "trial")
        open_map<blam::trial_version_t>(
            arguments.unmatched().at(0), arguments.unmatched().at(1));

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(dumper_main, 0x1 | 0x2)
