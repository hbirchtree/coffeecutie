#include <shader_proc/glslifier.h>

#include <shader_proc/optimizer.h>

#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/any_of.h>

#include <glslang/Public/ShaderLang.h>
#include <spirv_cross/spirv_glsl.hpp>

namespace shader_proc::glsl {

namespace {

void add_base_instance(spirv_cross::CompilerGLSL& compiler)
{
    compiler.add_header_line("uniform int glw_BaseInstance;");
    compiler.add_header_line(
        "#define gl_InstanceID (glw_BaseInstance + gl_InstanceID)");
}

void fix_base_instance(spirv_cross::CompilerGLSL& compiler)
{
    compiler.add_header_line(
        "#define gl_InstanceID (gl_BaseInstance + gl_InstanceID)");
}

void add_instance_definition(spirv_cross::CompilerGLSL& compiler)
{
    //    compiler.add_header_line("#define gl_InstanceID glw_InstanceID");
    //    compiler.add_header_line("uniform int glw_InstanceID;");
}

void remove_decorator_location(spirv_cross::CompilerGLSL& compiler)
{
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();
    for(spirv_cross::Resource const& buffer : resources.uniform_buffers)
    {
        compiler.unset_decoration(buffer.id, spv::DecorationBinding);
    }

    auto interface_variables = compiler.get_active_interface_variables();
    for(spirv_cross::VariableID const& id : interface_variables)
        compiler.unset_decoration(id, spv::DecorationLocation);
}

spv::ExecutionModel to_execution_model(typing::graphics::ShaderStage stage)
{
    using Stage = typing::graphics::ShaderStage;
    switch(stage)
    {
    case Stage::Vertex:
        return spv::ExecutionModelVertex;
    case Stage::Fragment:
        return spv::ExecutionModelFragment;
    case Stage::Compute:
        return spv::ExecutionModelGLCompute;
    case Stage::TessControl:
        return spv::ExecutionModelTessellationControl;
    case Stage::TessEval:
        return spv::ExecutionModelTessellationEvaluation;
    default:
        throw std::runtime_error("invalid stage");
    }
}

} // namespace

stl_types::result<std::string, glsl_error> generate(
    spv_input&& input, target_options&& output)
{
    using namespace std::string_literals;
    using Stage       = typing::graphics::ShaderStage;
    const bool legacy = output.version == 100;
    if(!stl_types::any_of(input.stage, Stage::Vertex, Stage::Fragment)
       && legacy)
        return stl_types::failure(std::make_pair(
            "invalid shader stage"s,
            "OpenGL SL 1.00 only supports vertex and fragment shaders"s));

    if(auto optimized = opt::perform_optimization(input, output);
       optimized.has_value())
        input.content = optimized.value();
    else
        return stl_types::failure(std::make_pair<std::string>(
            "failed to optimize", optimized.error()));

    spirv_cross::CompilerGLSL compiler(input.content);
    compiler.set_entry_point(
        std::string(output.entrypoint.begin(), output.entrypoint.end()),
        to_execution_model(input.stage));
    spirv_cross::CompilerGLSL::Options opts;

    opts.es      = output.profile == profile_t::es;
    opts.version = output.version;

    opts.vertex.support_nonzero_base_instance = false;

    const auto is_es        = output.profile == profile_t::es;
    const auto is_core      = output.profile == profile_t::core;
    const auto core_version = is_core ? output.version : 500;
    const auto es_version   = is_es ? output.version : 500;
    const auto is_vertex = input.stage == typing::graphics::ShaderStage::Vertex;

    compiler.set_variable_type_remap_callback(
        [es_version, core_version](
            spirv_cross::SPIRType const& /*type*/,
            std::string const& /*var_name*/,
            std::string& /*type_name*/) {
            if(es_version < 400 && es_version > 320)
                return;
            if(core_version < 490 && core_version > 430)
                return;
            // TODO: Flatten sampler2DArray
        });

    if((core_version < 460 || is_es || legacy) && is_vertex)
        add_base_instance(compiler);
    else if(is_vertex)
        fix_base_instance(compiler);
    if(core_version < 420 || es_version < 310 || legacy)
        remove_decorator_location(compiler);
    if(es_version < 310 || legacy)
        opts.force_flattened_io_blocks = true;
    if(legacy && is_vertex)
    {
        add_instance_definition(compiler);
        opts.emit_uniform_buffer_as_plain_uniforms = true;
    }
    compiler.set_common_options(opts);

    try
    {
        return stl_types::success(compiler.compile());
    } catch(std::exception const& exc)
    {
        return stl_types::failure(std::make_pair<std::string>(
            exc.what(), compiler.get_partial_source()));
    }
}

std::optional<std::string> validate_source(const std::string& /*source*/)
{
    return std::nullopt;
}

} // namespace shader_proc::glsl
