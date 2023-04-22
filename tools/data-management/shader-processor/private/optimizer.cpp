#include <shader_proc/optimizer.h>

#include <spirv-tools/optimizer.hpp>

#include <shader_proc/compiler_passes.h>

namespace shader_proc::opt {

stl_types::result<spv_blob, std::string> perform_optimization(
    spv_input const&      input,
    target_options const& out_options,
    optimizations const&  optimizations)
{
    spvtools::Optimizer        opt(SPV_ENV_OPENGL_4_0);
    spvtools::OptimizerOptions options;
    options.set_preserve_bindings(true);
    options.set_preserve_spec_constants(true);
    {
        spvtools::ValidatorOptions validation;
        validation.SetFriendlyNames(true);
        validation.SetUniformBufferStandardLayout(true);
        options.set_validator_options(validation);
    }
    if(optimizations.opt_level == optimization_level::fast)
        opt.RegisterPerformancePasses();
    else if(optimizations.opt_level == optimization_level::size)
        opt.RegisterSizePasses();

    passes::compatibility_options comp_options = {};
    if((out_options.profile == profile_t::core && out_options.version < 430)
       || (out_options.profile == profile_t::es && out_options.version < 310))
        comp_options.ssbo_as_ubo = true;
    if((out_options.profile == profile_t::core && out_options.version < 420)
       || out_options.profile == profile_t::es)
        comp_options.float64_to_float32 = true;
    if(out_options.version == 100)
    {
        comp_options.remove_flat_qualifier    = true;
        comp_options.unsigned_to_signed_int   = true;
        comp_options.int_stage_input_to_float = true;
    }
    opt.RegisterPass(
        passes::CreateLegacyCompatibilityPass(std::move(comp_options)));

    if(out_options.strip_debug)
    {
        opt.RegisterPass(spvtools::CreateStripDebugInfoPass());
    }

    if(out_options.targets_spv)
    {
        opt.RegisterPass(spvtools::CreateStripNonSemanticInfoPass())
            .RegisterPass(spvtools::CreateStripReflectInfoPass());
    }

    if(optimizations.rename_entrypoint)
    {
        opt.RegisterPass(passes::CreateRenameEntrypointPass(
            *optimizations.rename_entrypoint));
    }

    std::string messages;
    opt.SetMessageConsumer([&messages](
                               spv_message_level_t,
                               const char* a,
                               const spv_position_t&,
                               const char* message) {
        if(message)
        {
            messages.append(message);
            messages.push_back('\n');
        }
        if(a)
        {
            messages.append(a);
            messages.push_back('\n');
        }
    });

    spv_blob output;
    if(!opt.Run(input.content.data(), input.content.size(), &output, options))
        return stl_types::failure(messages);
    else
        return stl_types::success(output);
}

} // namespace shader_proc::opt
