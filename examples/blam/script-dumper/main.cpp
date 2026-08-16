
#include "blam/volta/blam_scenario.h"
#include "blam/volta/hsc/bytecode_common_v12.h"
#include "peripherals/stl/string/split.h"
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

using libc_types::u16;
using libc_types::i32;
using Coffee::Logging::cWarning;

template<blam::is_game_version Ver>
void open_map(std::string path, std::string output, std::string script)
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
    // Scripting
    auto bytecode = scenario->bytecode(map.magic);
    for(blam::hsc::global const& global : scenario->script.globals.data(map.magic).value())
    {
        auto name = global.name.str();
        auto type = magic_enum::enum_name(global.type);
        fprintf(fp, ";;   %04i: glob                %-32.*s type=%.*s\n",
            global.index,
            static_cast<int>(name.size()), name.data(),
            static_cast<int>(type.size()), type.data());
    }
    for(auto const& fun : scenario->function_table(map.magic))
    {
        auto name = fun.name.str();
        auto sched = magic_enum::enum_name(fun.schedule);
        auto type = magic_enum::enum_name(fun.type);
        fprintf(fp, ";; 0x%04x: func                %-32.*s schedule=%.*s return_type=%.*s\n",
            fun.index,
            static_cast<int>(name.size()), name.data(),
            static_cast<int>(sched.size()), sched.data(),
            static_cast<int>(type.size()), type.data());
    }
    // Cutscene data
    i32 i = 0;
    for(blam::scn::cutscene_camera_position const& cam : scenario->cutscene.camera_points.data(map.magic).value())
    {
        auto name = cam.name.str();
        fprintf(fp, ";;   %04i: cutscene_camera_pnt %-32.*s pos=%f,%f,%f rot=%f,%f,%f fov=%f\n",
            i++,
            static_cast<int>(name.size()), name.data(),
            cam.position.x, cam.position.y, cam.position.z,
            cam.rotation.x, cam.rotation.y, cam.rotation.z,
            cam.fov);
    }
    i = 0;
    for(blam::scn::cutscene_title const& title : scenario->cutscene.titles.data(map.magic).value())
    {
        auto name = title.name.str();
        fprintf(fp, ";;   %04i: cutscene_title      %-32.*s bounds=(top=%i,left=%i,bottom=%i,right=%i)\n",
            i++,
            static_cast<int>(name.size()), name.data(),
            title.text_bounds.x, title.text_bounds.y, title.text_bounds.z, title.text_bounds.w);
    }
    i = 0;
    for(blam::scn::cutscene_flag const& flag : scenario->cutscene.flags.data(map.magic).value())
    {
        auto name = flag.name.str();
        fprintf(fp, ";;   %04i: cutscene_flag       %-32.*s pos=%f,%f,%f facing=yaw=%f,pitch=%f\n",
            i++,
            static_cast<int>(name.size()), name.data(),
            flag.position.x, flag.position.y, flag.position.z,
            flag.facing.x, flag.facing.y);
    }
    // AI 
    i = 0;
    for(blam::scn::ai::conversation const& conv : scenario->ai.conversations.data(map.magic).value())
    {
        auto name = conv.name.str();
        fprintf(fp, ";;   %04i: ai conversation     %-32.*s participants=%u lines=%u\n",
            i++,
            static_cast<int>(name.size()), name.data(),
            conv.participants.size(),
            conv.lines.size());
        for(blam::scn::ai::conversation_participant const& p : conv.participants.data(map.magic).value())
        {
            auto name = p.encounter_name.str();
            fprintf(fp, ";;       : conversation_participant name=%.*s\n",
                static_cast<int>(name.size()), name.data());
        }
        for(blam::scn::ai::conversation_line const& l : conv.lines.data(map.magic).value())
        {
            fprintf(fp, ";;       : conversation_line\n");
        }
    }
    i = 0;
    for(blam::scn::ai::encounter const& enc : scenario->ai.encounters.data(map.magic).value())
    {
        auto name = enc.name.str();
        fprintf(fp, ";;   %04i: ai_encounter        %-32.*s squads=%u platoons=%u firing_positions=%u starting_locations=%u\n",
            i++,
            static_cast<int>(name.size()), name.data(),
            enc.squads.size(),
            enc.platoons.size(),
            enc.firing_positions.size(),
            enc.start_locations.size());
    }
    // Object references
    i = 0;
    for(blam::scn::object_name const& obname : scenario->objects.object_names.data(map.magic).value())
    {
        auto name = obname.name.str();
        fprintf(fp, ";;   %04i: object_name         \"%-32.*s\"\n",
            i++,
            static_cast<int>(name.size()), name.data());
    }
    // Misc data
    // TODO: Fix layout of editor_comment, it has variable-length strings
    // i = 0;
    // for(blam::scn::editor_comment const& comment 
    //     : scenario->editor.comments.data(map.magic).value())
    // {
    //     auto comment_ = comment.comment.str();
    //     fprintf(fp, ";;   %04i: comment pos=%f,%f,%f \"%-32.*s\"\n",
    //         i++,
    //         comment.position.x, comment.position.y, comment.position.z,
    //         static_cast<int>(comment_.size()), comment_.data());
    // }
    u16 ip{0};
    for(auto const& opcode : bytecode)
    {
        auto repr = blam::hsc::to_string(opcode);
        fprintf(fp, "0x%04x: %.*s | op=%04i\n",
            ip++,
            static_cast<int>(repr.size()), repr.data(),
            static_cast<int>(opcode.opcode));
    }
    fclose(fp);
    if(script.empty())
        return;
    fp = fopen(script.c_str(), "w+");
    auto script_text = blam::hsc::to_halo_script(*scenario, map.magic);
    for(auto const& line : stl_types::str::split::spliterator<char>(script_text, '\n'))
    {
        fprintf(fp, "%.*s\n", static_cast<int>(line.size()), line.data());
    }
    fclose(fp);
}

int dumper_main()
{
    cxxopts::Options options(
        "Blam Script Dumper",
        "An application to dump Halo Script from a .map file");
    options.add_options("Help")("h,help", "Show this help info");

    options.positional_help("[map file] [output file.hsd] [output file.hsc]");

    options.add_options("Map parsing")
        //
        ("halo-version", "Version of Halo: pc xbox custom trial", cxxopts::value<std::string>())
        //
        ;

    auto& args = Coffee::GetInitArgs();
    auto arguments = options.parse(args.size(), args.data());
    if(Coffee::BaseArgParser::PerformDefaults(options, args) >= 0)
        return 0;

    if(arguments.unmatched().size() < 2)
    {
        cWarning("Needs two arguments or three, check --help");
        std::quick_exit(1);
    }

    std::string selected_version =
        arguments.as_optional<std::string>("halo-version").value_or("pc");

    auto decomped = arguments.unmatched().at(1);
    std::string script_recomp;
    if(arguments.unmatched().size() >= 3)
        script_recomp = arguments.unmatched().at(2);

    if(selected_version == "pc")
        open_map<blam::pc_version_t>(
            arguments.unmatched().at(0), decomped, script_recomp);
    if(selected_version == "xbox")
        open_map<blam::xbox_version_t>(
            arguments.unmatched().at(0), decomped, script_recomp);
    if(selected_version == "custom")
        open_map<blam::custom_version_t>(
            arguments.unmatched().at(0), decomped, script_recomp);
    if(selected_version == "trial")
        open_map<blam::trial_version_t>(
            arguments.unmatched().at(0), decomped, script_recomp);

    std::quick_exit(0);
}

COFFEE_APPLICATION_MAIN_CUSTOM(dumper_main, 0x1 | 0x2)
