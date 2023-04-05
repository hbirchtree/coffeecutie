#include <coffee/application/application_start.h>
#include <coffee/core/files/cfiles.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/any_of.h>
#include <string_view>

#include <cxxopts.hpp>

#include <shader_proc/glslifier.h>
#include <shader_proc/optimizer.h>
#include <shader_proc/spvifier.h>

#include <coffee/core/debug/formatting.h>

using namespace Coffee::Logging;

typing::graphics::ShaderStage from_stage_string(
    std::string stage, std::string const& input_name)
{
    if(stage.empty())
        stage = platform::url::Path(input_name.data()).extension();

    using Stage = typing::graphics::ShaderStage;
    if(stage == "vert")
        return Stage::Vertex;
    if(stage == "frag")
        return Stage::Fragment;
    if(stage == "tess")
        return Stage::TessControl | Stage::TessEval;
    if(stage == "comp")
        return Stage::Compute;
    throw std::runtime_error("invalid stage specified: " + stage);
}

std::string to_stage_string(typing::graphics::ShaderStage stage)
{
    using Stage = typing::graphics::ShaderStage;
    if(stage == Stage::Vertex)
        return "vert";
    if(stage == Stage::Fragment)
        return "frag";
    if(stage == Stage::TessControl || stage == Stage::TessEval)
        return "tess";
    if(stage == Stage::Compute)
        return "comp";
    throw std::runtime_error("invalid stage specified");
}

shader_proc::profile_t from_profile_string(std::string_view profile)
{
    using namespace std::string_literals;
    if(profile == "es")
        return shader_proc::profile_t::es;
    if(profile == "core")
        return shader_proc::profile_t::core;
    throw std::runtime_error(
        "invalid profile: "s + std::string(profile.begin(), profile.end()));
}

shader_proc::profile_t from_profile_string(std::string const& profile)
{
    return from_profile_string(
        std::string_view(profile.data(), profile.size()));
}

std::optional<std::pair<uint32_t, shader_proc::profile_t>> from_source(
    std::string_view source_code)
{
    using namespace std::string_view_literals;
    auto version_string = source_code.find("#version");
    if(version_string == std::string_view::npos)
        return std::nullopt;
    auto version_end = source_code.find_first_of("\n", version_string);
    if(version_end == std::string_view::npos)
        return std::nullopt;
    auto version_data = source_code.substr(
        version_string + "#version "sv.size(),
        version_end - version_string - "#version "sv.size());
    auto version
        = stl_types::cast_string_view<uint32_t>(version_data.substr(0, 3));
    version_data = version_data.substr(3);
    if(version_data.size() < 3)
        return std::make_pair(version, shader_proc::profile_t::none);
    auto profile = version_data.substr(1, version_end);
    return std::make_pair(version, from_profile_string(profile));
}

std::string make_c_identifier(std::string dest)
{
    for(auto ch : std::array<char, 5>{{'-', ',', '.', '+', '*'}})
        std::erase(dest, ch);
    return dest;
}

libc_types::i32 cooker_main()
{
    using namespace platform::url::constructors;
    using libc_types::u32;
    using stl_types::cast_string;

    std::vector<const char*> arguments;
    arguments.push_back("ShaderCooker");
    for(auto arg : Coffee::GetInitArgs().arguments())
        arguments.push_back(arg);

    cxxopts::Options opts("ShaderCooker", "SPIR-V shader processor");
    opts.positional_help("shader files")
        .add_options()
        //
        ("s,stage",
         "Shader stage for single-shader processing",
         cxxopts::value<std::string>())
        //
        ("p,profile",
         "Output GLSL profile for GLSL generation",
         cxxopts::value<std::string>())
        //
        ("V,version",
         "Output GLSL version for GLSL generation",
         cxxopts::value<std::string>())
        //
        ("o,output",
         "Output file when using single files and -M",
         cxxopts::value<std::string>())
        //
        ("D,define",
         "Add preprocessor defines before compiling",
         cxxopts::value<std::string>())
        //
        ("f,force", "Overwrite output files if already existing")
        //
        ("B,spirv", "Generate SPIR-V binary from input files")
        //
        ("strip-debug", "Strip debug information from SPIR-V binary")
        //
        ("M,library",
         "Combine SPIR-V binaries into a large module (only if -B is "
         "specified)");
    opts.allow_unrecognised_options();

    auto res = opts.parse(arguments.size(), arguments.data());

    if(res.unmatched().size() < 1)
    {
        cFatal("No input files specified");
        return 1;
    }
    if(res.count("spirv") == 1 && res.count("output") != 1)
    {
        cFatal("No outputs specified for SPV binary");
        return 1;
    }
    if(res.unmatched().size() != res.count("stage"))
    {
        cFatal("Number of stages does not match number of files");
        return 1;
    }
    if(res.unmatched().size() != res.count("output")
       && !(res.count("spirv") && res.count("library")))
    {
        cFatal("Number of outputs does not match number of files");
        return 1;
    }

    std::vector<std::pair<typing::graphics::ShaderStage, std::string>> inputs;
    for(auto const& file : res.unmatched())
        inputs.push_back(std::make_pair(from_stage_string({}, file), file));

    std::map<std::string, std::string> defines;
    for(auto const& arg : res)
    {
        using namespace std::string_view_literals;
        if(arg.key() != "define")
            continue;
        std::string define = arg.value();
        if(auto separator = define.find('='); separator != std::string::npos)
        {
            std::string name  = define.substr(0, separator);
            std::string value = define.substr(separator + 1);
            defines.insert(std::make_pair(name, value));
        } else
            defines.insert(std::make_pair(define, "1"));
    }

    std::vector<shader_proc::spv_blob> blobs;
    for(auto const& [stage, file] : inputs)
    {
        Coffee::Resource source_file(MkUrl(file));
        auto             source_data = source_file.data();
        std::string_view source_code(source_data.data(), source_data.size());

        auto [version, profile]
            = from_source(source_code)
                  .value_or(std::make_pair(460u, shader_proc::profile_t::core));

        // First, compile to SPV
        auto spv = shader_proc::spirv::compile({
            .stage   = stage,
            .content = source_code,
            .path    = file,
            .version = version,
            .profile = profile,
            .defines = defines,
        });

        if(spv.has_error())
        {
            cBasicPrint("{0}: {1}", file, std::get<1>(spv.error()));
            return 1;
        }
        blobs.push_back(std::move(spv.value()));
    }

    using semantic::RSCA;

    if(res.count("spirv"))
    {
        using namespace std::string_view_literals;

        // After successful compilation, optimize the SPV binaries
        const bool                         create_module = res.count("library");
        std::vector<shader_proc::spv_blob> linkables;
        size_t                             i = 0;
        for(auto&& blob : blobs)
        {
            auto stage      = inputs[i].first;
            auto entrypoint = make_c_identifier(
                platform::url::Path(inputs[i].second)
                    .fileBasename()
                    .removeExt()
                    .internUrl
                + "_main");
            auto optimized = shader_proc::opt::perform_optimization(
                {
                    .content = std::move(blob),
                    .stage   = stage,
                },
                {
                    .profile     = shader_proc::profile_t::core,
                    .version     = 460,
                    .strip_debug = static_cast<bool>(res.count("strip-debug")),
                },
                {
                    .rename_entrypoint = std::string_view(entrypoint.c_str()),
                });

            if(optimized.has_error())
            {
                cBasicPrint("{0}: {1}", inputs[i].second, optimized.error());
                return 1;
            } else if(create_module)
            {
                linkables.push_back(std::move(optimized.value()));
                ++i;
                continue;
            }

            auto out_file = platform::url::Path(inputs[i].second)
                                .removeExt()
                                .addExtension("spv")
                                .url();
            Coffee::Resource out_spv(out_file);
            if(Coffee::FileExists(out_spv) && res.count("force") == 0)
            {
                cBasicPrint(
                    "File {0} exists, but -f is not specified", out_file);
                return 1;
            }
            out_spv = semantic::BytesConst::ofContainer(optimized.value());
            Coffee::FileCommit(
                out_spv,
                RSCA::NewFile | RSCA::WriteOnly | RSCA::Truncate
                    | (res.count("force") ? RSCA::Discard : RSCA::None));
            ++i;
        }

        if(!create_module)
            return 0;

        auto lib = shader_proc::spirv::link(std::move(linkables));
        if(lib.has_error())
        {
            cBasicPrint("{0}", lib.error());
            return 1;
        }
        if(auto opt = shader_proc::opt::perform_optimization(
               {
                   .content = std::move(lib.value()),
                   .stage   = typing::graphics::ShaderStage::All,
               },
               {
                   .profile     = shader_proc::profile_t::core,
                   .version     = 460,
                   .strip_debug = false,
               });
           opt.has_error())
        {
            cBasicPrint("{0}", opt.error());
            return 1;
        } else
            lib = opt.value();
        Coffee::Resource out_spv(MkUrl(res["output"].as<std::string>()));

        if(Coffee::FileExists(out_spv) && !res.count("force"))
        {
            cBasicPrint(
                "File {0} exists, but -f is not specified",
                res["output"].as<std::string>());
            return 1;
        }

        out_spv = semantic::BytesConst::ofContainer(lib.value());
        Coffee::FileCommit(
            out_spv,
            RSCA::NewFile | RSCA::WriteOnly | RSCA::Truncate | RSCA::Discard);
        return 0;
    } else
    {
        // Then create GLSL from it
        size_t                   i = 0;
        std::vector<std::string> outputs;
        for(auto const& kv : res.arguments())
            if(kv.key() == "output")
                outputs.push_back(kv.value());
        for(auto&& blob : blobs)
        {
            auto stage = inputs[i].first;
            auto glsl  = shader_proc::glsl::generate(
                {
                     .content = std::move(blob),
                     .stage   = stage,
                },
                {
                     .profile
                    = from_profile_string(res["profile"].as<std::string>()),
                     .version
                    = cast_string<u32>(res["version"].as<std::string>()),
                });
            if(glsl.has_error())
            {
                auto [error, src] = glsl.error();
                cBasicPrint(
                    "{0}:0: {1}\n^~~ After emitting:\n{2}",
                    outputs[i],
                    error,
                    src);
                return 1;
            } else
            {
                Coffee::Resource output(MkUrl(outputs[i]));
                output = semantic::BytesConst::ofContainer(glsl.value());
                auto overwrite_flag
                    = res.count("force") ? RSCA::Discard : RSCA::None;
                if(Coffee::FileExists(output) && overwrite_flag == RSCA::None)
                {
                    cBasicPrint(
                        "File {0} exists, but -f is not specified", outputs[i]);
                    return 1;
                }
                Coffee::FileCommit(
                    output,
                    RSCA::NewFile | RSCA::WriteOnly | RSCA::Truncate
                        | overwrite_flag);
            } // else
              // cBasicPrint("{0}", glsl.value());
        }
        return 0;
    }
}

// Add DiscardArgumentHandler and SilentInit flag
COFFEE_APPLICATION_MAIN_CUSTOM(cooker_main, 0x1 | 0x2)
