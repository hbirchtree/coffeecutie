#include <shader_proc/spvifier.h>

#include <shaderc/shaderc.hpp>
#include <spirv-tools/linker.hpp>

#include <platforms/file.h>

namespace shader_proc::spirv {
namespace {

class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
{
    struct include_data
    {
        std::string                filename;
        platform::file::map_handle mapping;
        shaderc_include_result     result;
    };

  public:
    shaderc_include_result* GetInclude(
        const char* requested_source,
        shaderc_include_type /*type*/,
        const char* requesting_source,
        size_t /*include_depth*/)
    {
        auto requested_path  = platform::url::Path(requested_source);
        auto requesting_path = platform::url::Path(requesting_source);

        auto lookup_path = requesting_path.dirname() / requested_path;

        if(auto mapping = platform::file::map(
               lookup_path.url(),
               {
                   .access = semantic::RSCA::ReadOnly,
               });
           mapping.has_error())
        {
            fprintf(
                stderr,
                "%s:0: file \"%s\" not found\n",
                requesting_source,
                requested_source);
            return &error;
        } else
        {
            auto const& data = mapping.value().view;
            m_cache.push_back(include_data{
                .filename = lookup_path.internUrl,
                .mapping  = std::move(mapping.value()),
                .result   = {
                    .source_name = nullptr,
                    .source_name_length = 0,
                    .content = data.data(),
                    .content_length = data.size_bytes(),
                    .user_data = nullptr,
                },
            });
            for(include_data& res : m_cache)
                if(res.filename == lookup_path.internUrl)
                {
                    res.result.source_name        = res.filename.data();
                    res.result.source_name_length = res.filename.size();
                    return &res.result;
                }
            {
                fprintf(
                    stderr,
                    "%s:0: file \"%s\" not found\n",
                    requesting_source,
                    requested_source);
                return &error;
            }
        }
    }
    void ReleaseInclude(shaderc_include_result* data)
    {
        auto it = std::find_if(
            m_cache.begin(), m_cache.end(), [data](include_data const& d) {
                return d.filename == data->source_name;
            });
        if(it != m_cache.end())
            m_cache.erase(it);
    }

  private:
    std::list<include_data> m_cache;
    shaderc_include_result  error{};
};

} // namespace

shaderc_shader_kind to_shaderc(typing::graphics::ShaderStage stage)
{
    using Stage = typing::graphics::ShaderStage;
    switch(stage)
    {
    case Stage::Vertex:
        return shaderc_vertex_shader;
    case Stage::Fragment:
        return shaderc_fragment_shader;
    case Stage::Compute:
        return shaderc_compute_shader;
    default:
        throw std::runtime_error("type not implemented");
    }
}

shaderc_profile to_shaderc(profile_t profile)
{
    switch(profile)
    {
    case profile_t::core:
        return shaderc_profile_core;
    case profile_t::es:
        return shaderc_profile_es;
    default:
        return shaderc_profile_none;
    }
}

stl_types::result<spv_blob, spv_error> compile(
    shader_input&&              shader,
    output_options const&       opts,
    optimization_options const& opt_opts)
{
    static thread_local shaderc::Compiler compiler;

    shaderc::CompileOptions options;
    options.SetTargetEnvironment(
        opts.env == environment_t::opengl ? shaderc_target_env_opengl
                                          : shaderc_target_env_vulkan,
        opts.version);
    options.SetWarningsAsErrors();
    options.SetSourceLanguage(shaderc_source_language_glsl);
    options.SetForcedVersionProfile(shader.version, to_shaderc(shader.profile));
    options.SetGenerateDebugInfo();
    options.SetIncluder(std::make_unique<ShaderIncluder>());
    for(auto const& [name, value] : shader.defines)
        options.AddMacroDefinition(name, value);

    switch(opt_opts.opt_level)
    {
    case shader_proc::opt::optimization_level::fast:
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        break;
    case shader_proc::opt::optimization_level::size:
        options.SetOptimizationLevel(shaderc_optimization_level_size);
        break;
    default:
        options.SetOptimizationLevel(shaderc_optimization_level_zero);
        break;
    }

    std::string path(shader.path.begin(), shader.path.end());

    auto result = compiler.CompileGlslToSpv(
        std::string(shader.content.begin(), shader.content.end()),
        to_shaderc(shader.stage),
        path.data(),
        options);

    switch(result.GetCompilationStatus())
    {
    case shaderc_compilation_status_success:
        break;
    default:
        return stl_types::failure(std::make_pair(0, result.GetErrorMessage()));
    }

    spv_blob out(result.begin(), result.end());
    return stl_types::success(out);
}

stl_types::result<spv_blob, spv_error> link(std::vector<spv_blob>&& blobs)
{
    using namespace std::string_literals;
    spvtools::Context context(spv_target_env::SPV_ENV_OPENGL_4_5);
    context.SetMessageConsumer([](spv_message_level_t,
                                  const char*           source,
                                  const spv_position_t& position,
                                  const char*           message) {
        fprintf(
            stderr,
            "%s:%zu:%zu: %s\n",
            source,
            position.line,
            position.column,
            message);
    });
    spvtools::LinkerOptions options = {};
    options.SetVerifyIds(true);

    spv_blob output;
    if(auto res = spvtools::Link(context, blobs, &output, options);
       res == SPV_SUCCESS || res == SPV_WARNING)
        return stl_types::success(output);
    else
        return stl_types::failure(std::make_pair(0, "Linker error"s));
}

} // namespace shader_proc::spirv
