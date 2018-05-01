#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/string_casting.h>
#include <coffee/interfaces/content_pipeline.h>

#include <coffee/core/CDebug>
#include <coffee/core/terminal/terminal_cursor.h>

#include <glslang/Public/ShaderLang.h>
#include <shaderc/shaderc.hpp>

#if defined(HAVE_SPIRVCROSS)
#include <libspirv.hpp>
#include <optimizer.hpp>
#include <spirv-tools/opt/pass.h>
#include <spirv_glsl.hpp>
#endif

#define SHD_API "PressurizeShaders::"

using namespace CoffeePipeline;
using namespace Coffee;

using Pass      = spvtools::opt::Pass;
using PassToken = spvtools::Optimizer::PassToken;

static Vector<CString> shader_extensions = {
    "vert", "frag", "geom", "tesc", "tese", "comp"};

static Map<CString, shaderc_shader_kind> shader_mapping = {
    {"vert", shaderc_glsl_vertex_shader},
    {"frag", shaderc_glsl_fragment_shader},
    {"geom", shaderc_glsl_geometry_shader},
    {"tesc", shaderc_glsl_tess_control_shader},
    {"tese", shaderc_glsl_tess_evaluation_shader},
    {"comp", shaderc_glsl_compute_shader},
};

struct GLSLTarget
{
    u32  version;
    bool es_sl;
};

struct CoffeeIncluder : shaderc::CompileOptions::IncluderInterface
{
    LinkList<shaderc_include_result> m_results;

  public:
    virtual shaderc_include_result* GetInclude(
        const char*          requested_source,
        shaderc_include_type type,
        const char*          requesting_source,
        size_t)
    {
        /* TODO: Extend with stock shaders for #include <A.h> */
        switch(type)
        {
        case shaderc_include_type_relative:
        {
            Path request_file(requesting_source);
            request_file = request_file.dirname() + requested_source;

            Resource* r = new Resource(MkUrl(request_file, RSCA::AssetFile));

            if(!FileExists(*r))
            {
                return nullptr;
            }

            Bytes data = C_OCAST<Bytes>(*r);

            m_results.emplace_back();
            auto& res = m_results.back();

            res.content        = C_FCAST<cstring>(data.data);
            res.content_length = data.size;

            res.source_name        = requested_source;
            res.source_name_length = StrLen(requested_source);

            res.user_data = r;

            return &res;
        }
        default:
            return nullptr;
        }
    }
    virtual void ReleaseInclude(shaderc_include_result* data)
    {
        auto pred = [&](shaderc_include_result& r) {
            if(&r == data)
            {
                delete C_RCAST<Resource*>(r.user_data);
                return true;
            }
            return false;
        };

        auto removeIt =
            std::remove_if(m_results.begin(), m_results.end(), pred);

        m_results.erase(removeIt, m_results.end());
    }
};

struct LegacyTransform : Pass
{
    struct LegacyOptions
    {
        bool allow_matrix_in_block  = false;
        bool allow_integer_varyings = false;
        bool allow_unsigned_integer = false;
        bool allow_flat_decorator   = false;
        bool flatten_all_samplers   = true;
    } options;

  public:
    LegacyTransform()
    {
    }

    virtual const char* name() const
    {
        return "legacy-transform";
    }

    struct type_map_t
    {
        type_map_t() :
            type_parent(nullptr), op_type(SpvOpTypeVoid), components(1)
        {
        }

        type_map_t* type_parent;

        SpvOp op_type;
        u32   components;

        u8 _pad[4];

        inline bool is_equivalent(type_map_t const& other) const
        {
            if(type_parent && other.type_parent &&
               components == other.components)
                return type_parent->is_equivalent(*other.type_parent);

            return components == other.components;
        }
    };

  protected:
    bool isValidBlockType(type_map_t const* type_id)
    {
        /* Check that composite type is valid, if applicable */
        switch(type_id->op_type)
        {
        case SpvOpTypeMatrix:
            if(!options.allow_matrix_in_block)
                return false;
            break;
        case SpvOpTypeInt:
            if(!options.allow_integer_varyings)
                return false;
            break;
        default:
            break;
        }

        if(type_id->type_parent)
            return isValidBlockType(type_id->type_parent);

        return true;
    }

    /*!
     * \brief This function handles rewrites of ``varying int'' into ``varying
     * float''. In the vertex shader, all stores will be wrapped with casts, in
     * the fragment shader there will be casts from float to int. All we can
     * hope is that there is no interpolation, because there are no ``flat''
     * decorators  in GLSL 1.00.
     *
     * \param context
     * \param inst
     * \param changed_types
     * \param valid_types
     * \return
     */
    bool rewriteAccessChain(
        spvtools::ir::IRContext*    context,
        spvtools::ir::Instruction*  inst,
        Map<u32, u32> const&        changed_types,
        Map<u32, type_map_t> const& valid_types)
    {
        using namespace spvtools;

        bool changed = false;

        /* This operand points to the type definition for the
         *  Output parameter */
        auto typeDefOp =
            context->get_def_use_mgr()->GetDef(inst->GetOperand(0).words.at(0));

        auto  storageClass = typeDefOp->GetSingleWordInOperand(0);
        auto& typeRef      = typeDefOp->GetInOperand(1);

        /* We only change outputs */
        if(storageClass != SpvStorageClassOutput &&
           storageClass != SpvStorageClassInput)
            return false;

        if(this->isValidBlockType(&valid_types.at(typeRef.words.at(0))))
            return false;

        auto it = changed_types.find(typeRef.words.at(0));

        if(it == changed_types.end())
            return false;

        /* If the underlying type has changed,
         *  change the OpAccessChain type */
        typeRef.words.at(0) = it->second;
        changed             = true;

        auto accessRewrite = [&](ir::Instruction* user, u32) {
            /* Addresses created with OpAccessChain are only
             *  valid with OpStore/OpLoad, so we only need
             * to care about these two cases. */

            bool isLoad = user->opcode() == SpvOpLoad;

            u32 addressOperand = isLoad ? 0 : 1;

            auto newId = context->TakeNextId();

            /* The constructor for ir::Instruction will not
             * let us create a valid instruction, so we use
             * the C API to create one with all the
             * operands. */
            spv_parsed_instruction_t convertOp;
            convertOp.opcode = isLoad ? SpvOpConvertFToS : SpvOpConvertSToF;
            convertOp.num_operands = 3;
            convertOp.num_words    = 3;
            convertOp.type_id      = SPV_OPERAND_TYPE_ID;
            convertOp.result_id    = newId;

            Vector<spv_parsed_operand_t> operands;
            Vector<u32>                  words;

            operands.resize(3);
            words.resize(3);

            convertOp.words    = words.data();
            convertOp.operands = operands.data();

            operands[0].num_words = 1;
            operands[0].offset    = 0;
            operands[0].type      = SPV_OPERAND_TYPE_TYPE_ID;
            words[0]              = isLoad ? it->first : it->second;

            operands[1].num_words = 1;
            operands[1].offset    = 1;
            operands[1].type      = SPV_OPERAND_TYPE_RESULT_ID;
            words[1]              = newId;

            operands[2].num_words = 1;
            operands[2].offset    = 2;
            operands[2].type      = SPV_OPERAND_TYPE_ID;
            words[2] = user->GetInOperand(addressOperand).words.at(0);

            auto castInstruction = MkUq<ir::Instruction>(context, convertOp);

            auto& storeSourceOp       = user->GetInOperand(addressOperand);
            storeSourceOp.words.at(0) = newId;
            changed                   = true;

            user->InsertBefore(std::move(castInstruction));
        };

        /* Find all later uses from the expression and apply casts
         */
        context->get_def_use_mgr()->ForEachUse(inst, accessRewrite);

        return changed;
    }

    virtual Status Process(spvtools::ir::IRContext* context)
    {
        using namespace spvtools;

        bool changed = false;

        Map<u32, type_map_t> valid_types;
        Map<u32, u32>        changed_types;

        context->module()->ForEachInst([&](ir::Instruction* inst) {
            /* Turn all samplers into sampler2D */
            if(inst->opcode() == SpvOpTypeImage && options.flatten_all_samplers)
            {
                inst->GetInOperand(3).words.at(0) = 0;
                inst->GetInOperand(4).words.at(0) = 0;

                changed = true;
            }

            /* Make all ints signed */
            if(inst->opcode() == SpvOpTypeInt)
            {
                inst->GetInOperand(1).words.at(0) = 1;
                changed                           = true;
            }

            /* Turn all floating-point types into float */
            if(inst->opcode() == SpvOpTypeFloat)
            {
                changed = inst->GetInOperand(0).words.at(0) != 32;
                inst->GetInOperand(0).words.at(0) = 32;
            }

            /* Remove flat decorators */
            if(inst->opcode() == SpvOpMemberDecorate ||
               inst->opcode() == SpvOpDecorate)
            {
                u32 decorator = 0;

                if(inst->opcode() == SpvOpDecorate)
                    decorator = inst->GetInOperand(1).words.at(0);
                else
                    decorator = inst->GetInOperand(2).words.at(0);

                switch(decorator)
                {
                case SpvDecorationFlat:
                    changed = true;
                    inst->ToNop();
                    break;
                default:
                    break;
                }
            }

            /* For types in Block, change the type to float-based ones.
             *  GLSL 1.00 does not support ints as varying. */
            if(inst->opcode() > SpvOpTypeVoid &&
               inst->opcode() < SpvOpTypeStruct)
            {
                u32        op        = inst->GetOperand(0).words.at(0);
                type_map_t type_desc = {};
                type_desc.op_type    = inst->opcode();

                u32 type_ref = 0;

                switch(inst->opcode())
                {
                case SpvOpTypeVector:
                    type_ref             = inst->GetInOperand(0).words.at(0);
                    type_desc.components = inst->GetInOperand(1).words.at(0);
                    break;
                case SpvOpTypeMatrix:
                    type_ref             = inst->GetInOperand(0).words.at(0);
                    type_desc.components = inst->GetInOperand(1).words.at(0);
                    break;
                default:
                    break;
                }

                if(type_ref != 0)
                    type_desc.type_parent = &valid_types[type_ref];

                valid_types.emplace(op, type_desc);
            }
            if(inst->opcode() == SpvOpTypeStruct)
            {
                for(auto i : Range<>(inst->NumInOperands()))
                    do
                    {
                        auto& op      = inst->GetInOperand(i);
                        u32&  type_id = op.words.at(0);
                        auto  it      = valid_types.find(type_id);
                        auto  pred    = [&](Pair<u32, type_map_t> const& type) {
                            return type.first != it->first &&
                                   type.second.is_equivalent(it->second);
                        };

                        /* Check the detected type recursively for validity,
                         * vectors and matrices link to their underlying type.
                         * If the underlying type is int, we need to change it.
                         */
                        if(it == valid_types.end() ||
                           this->isValidBlockType(&it->second))
                            break;

                        auto remap_type = std::find_if(
                            valid_types.begin(), valid_types.end(), pred);

                        if(remap_type == valid_types.end())
                            break;

                        /* Once the type has been changed,
                         *  the dependent variables need to be changed too. */
                        changed_types[type_id] = remap_type->first;
                        type_id                = remap_type->first;
                        changed                = true;
                    } while(false);
            }

            /* We find all loads from output blocks here */
            if(inst->opcode() == SpvOpAccessChain)
            {
                changed |= this->rewriteAccessChain(
                    context, inst, changed_types, valid_types);
            }
        });

        if(!changed)
            return Status::SuccessWithoutChange;

        //        context->module()->ForEachInst([](ir::Instruction* inst) {
        //            cBasicPrint("{0}", inst->PrettyPrint());
        //        });

        return Status::SuccessWithChange;
    }
};

static Vector<u32> CompileSpirV(
    TerminalCursor&            cursor,
    shaderc::Compiler&         compiler,
    CString const&             entrypoint,
    Bytes const&               source,
    Path const&                path,
    shaderc_optimization_level level,
    shaderc_target_env         target_env = shaderc_target_env_opengl,
    GLSLTarget const&          target     = {450, false})
{
    shaderc::CompileOptions options;

    options.SetIncluder(MkUq<CoffeeIncluder>());
    options.SetOptimizationLevel(level);
    options.SetSourceLanguage(shaderc_source_language_glsl);

    /* We might want to create Vulkan SPV in the future */
    options.SetTargetEnvironment(target_env, 0);

    /* We want to avoid setting locations manually */
    options.SetAutoMapLocations(true);

    shaderc_profile p =
        target.es_sl ? shaderc_profile_es : shaderc_profile_core;

    if((!target.es_sl && target.version < 400) ||
       (target.es_sl && target.version < 300))
        p = shaderc_profile_none;

    options.SetForcedVersionProfile(C_FCAST<i32>(target.version), p);

    auto type = shader_mapping[path.extension()];

    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
        C_RCAST<const char*>(source.data),
        source.size,
        type,
        path.internUrl.c_str(),
        entrypoint.c_str(),
        options);

    auto result = module.GetCompilationStatus();

    if(result != shaderc_compilation_status_success)
    {
        cursor.print(
            SHD_API
            R"(Shader compilation failed ({1}) with:
{0}
Associated source:
{2})",
            module.GetErrorMessage(),
            path,
            StrUtil::encapsulate(
                C_RCAST<const char*>(source.data), source.size));
        cursor.progress(SHD_API "Skipping shader");
        return {};
    }

    cursor.progress(
        SHD_API "Shader compiled: GLSL:{1}:{2} {0}",
        path.internUrl,
        target.version,
        ((target.es_sl) ? "es" : "core"));
    return {module.begin(), module.end()};
}

static Vector<u32> TransformToLegacy(Vector<u32> const& source)
{
    spvtools::SpirvTools core(SPV_ENV_OPENGL_4_0);
    spvtools::Optimizer  opt(SPV_ENV_OPENGL_4_0);

    auto logger = [](spv_message_level_t,
                     const char*,
                     const spv_position_t&,
                     const char* m) { cWarning("{0}", m); };

    core.SetMessageConsumer(logger);
    opt.SetMessageConsumer(logger);

    opt.RegisterPass(spvtools::CreateFlattenDecorationPass())
        .RegisterPass(spvtools::CreateDeadVariableEliminationPass())
        .RegisterPass(spvtools::CreateEliminateDeadConstantPass())
        .RegisterPass(spvtools::CreateDeadBranchElimPass())
        .RegisterPass(spvtools::CreateLoopUnrollPass(true))
        .RegisterPass(spvtools::CreateMergeReturnPass())
        .RegisterPass(PassToken::CreateWrap(
            UqPtr<Pass>(dynamic_cast<Pass*>(new LegacyTransform))));

    Vector<u32> output;
    if(!opt.Run(source.data(), source.size(), &output))
        cWarning("Failure!");

    //    CString dism;
    //    if(core.Disassemble(output, &dism))
    //        cDebug("\n{0}", dism);

    return output;
}

static void GenerateGLSL(
    TerminalCursor&           cursor,
    Vector<VirtFS::VirtDesc>& files,
    Path const&               srcFile,
    Vector<u32> const&        source,
    GLSLTarget const&         target)
{
#if defined(HAVE_SPIRVCROSS)
    Vector<u32> const* sourceRef = &source;
    Vector<u32>        localSource;
    /* Do a pass over the source with SPIRV-Tools to create valid GLSL 1.00 */
    if(target.es_sl && target.version == 100)
    {
        localSource = TransformToLegacy(source);
        sourceRef   = &localSource;
    }

    spirv_cross::CompilerGLSL compiler(sourceRef->data(), sourceRef->size());

    spirv_cross::ShaderResources rsc = compiler.get_shader_resources();

    /* SPIRV-Cross does not recognize gl_BaseInstance */
    compiler.add_header_line("#define gl_BuiltIn_4425 gl_BaseInstance");

    //        /* Generate grid uniform for every array texture */
    //        for(auto const& img : rsc.sampled_images)
    //        {
    //            auto img_type = compiler.get_type_from_variable(img.id);

    //            if(img_type.image.dim == spv::Dim2D &&
    //            !img_type.image.arrayed)
    //                continue;

    //            compiler.add_header_line(
    //                cStringFormat("uniform int {0}_GridSize;", img.name));
    //        }

    /* In GLSL 4.60+, BaseInstance is real */
    if(target.version < 460)
    {
        compiler.add_header_line("#define gl_BaseInstance BaseInstance");
        compiler.add_header_line("#define gl_BaseInstance BaseInstance");
    }

    /* In GLSL 4.60+, gl_BaseInstance should not be
     *  suffixed with *ARB, right? */
    if(target.version == 460)
    {
        compiler.add_header_line("#define gl_BaseInstanceARB"
                                 " gl_BaseInstance");
    }

    spirv_cross::CompilerGLSL::Options opts;
    opts.version = target.version;
    opts.es      = target.es_sl;

    /* When compiling for GLSL 4.10+, we always use SSO */
    if(!target.es_sl && target.version >= 410)
        opts.separate_shader_objects = true;

    compiler.set_common_options(opts);

    CString glsl;

    /* Compilation may fail for various reasons.
     *  We will report the error and not emit any GLSL. */
    try
    {
        glsl = compiler.compile();
        cursor.progress(
            SHD_API "Shader code generated: GLSL:{0}:{1} {2}",
            target.version,
            target.es_sl ? "es" : "core",
            srcFile.internUrl);
    } catch(spirv_cross::CompilerError const& e)
    {
        cursor.print(
            "{0}:{1}:{2}: {3}\n{4}",
            GetFileResourcePrefix() + "/" + srcFile.internUrl,
            0,
            Stacktracer::DemangleSymbol(typeid(e).name()),
            e.what(),
            compiler.get_partial_source());
        return;
    }

    /* We take a round-trip to shaderc for validation
     *  of the generated GLSL */
    if(!target.es_sl)
    {
        shaderc::Compiler scompiler;
        auto              data = CompileSpirV(
            cursor,
            scompiler,
            srcFile.extension(),
            Bytes::CreateString(glsl.c_str()),
            srcFile,
            shaderc_optimization_level_zero,
            shaderc_target_env_opengl,
            target);

        /* If validation step fails, do not add the GLSL. It is invalid. */
        if(data.size() == 0)
            return;
    }

    Vector<u8> glslData(glsl.begin(), glsl.end());

    auto ext  = srcFile.extension();
    auto path = srcFile.removeExt()
                    .addExtension(opts.es ? "es" : "core")
                    .addExtension(cast_pod(opts.version).c_str())
                    .addExtension(ext.c_str());

    files.push_back({path.internUrl.c_str(),
                     Bytes::CopyFrom(glslData),
                     VirtFS::File_Compressed});
#endif
}

struct ShaderProcessor : FileProcessor
{
    virtual void process(
        Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor);

    virtual void setBaseDirectories(const Vector<CString>&)
    {
    }
};

COFFAPI FileProcessor* CoffeeLoader()
{
    return new ShaderProcessor;
}

void ShaderProcessor::process(
    Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor)
{
    shaderc::Compiler compiler;

    Map<Path, const VirtFS::VirtDesc*> selection;

    for(auto const& file : files)
    {
        Path p = Path::Mk(file.filename.c_str());

        auto it = std::find(
            shader_extensions.begin(), shader_extensions.end(), p.extension());

        if(it == shader_extensions.end())
            continue;

        selection.emplace(p, &file);
    }

    cursor.progress(SHD_API "Found {0} shaders", selection.size());

    Vector<VirtFS::VirtDesc> newFiles;
    newFiles.reserve(selection.size() * 5);

    for(auto const& path : selection)
    {
        /* Generate optimized SPIR-V binary */
        Vector<u32> optimized = CompileSpirV(
            cursor,
            compiler,
            path.first.extension(),
            path.second->data,
            path.first,
            shaderc_optimization_level_performance);

        if(optimized.size() == 0)
        {
            cursor.print(
                "{0}:0: Failed to compile GLSL to SPIR-V",
                (Path(GetFileResourcePrefix()) + path.first).internUrl);
            continue;
        }

        Path binaryPath = path.first.addExtension("spv");

        newFiles.push_back({binaryPath.internUrl.c_str(),
                            Bytes::CopyFrom(optimized),
                            VirtFS::File_Compressed});

        /* Generate unoptimized SPIR-V binary for
         *  recompilation to GLSL */
        Vector<u32> unoptimized = CompileSpirV(
            cursor,
            compiler,
            path.first.extension(),
            path.second->data,
            path.first,
            shaderc_optimization_level_zero,
            shaderc_target_env_opengl,
            {460, false});

        if(unoptimized.size() == 0)
        {
            cursor.progress(SHD_API "Failed to create unoptimized binary");
            continue;
        }

        /* With the generated SPIR-V, cross-compile it to
         *  various GLSL/ES versions, for portability */
        GenerateGLSL(cursor, newFiles, path.first, unoptimized, {100, true});
        GenerateGLSL(cursor, newFiles, path.first, unoptimized, {300, true});

        GenerateGLSL(cursor, newFiles, path.first, unoptimized, {330, false});
        GenerateGLSL(cursor, newFiles, path.first, unoptimized, {430, false});
        GenerateGLSL(cursor, newFiles, path.first, unoptimized, {460, false});
    }

    files.reserve(files.size() + newFiles.size());

    for(auto& desc : newFiles)
    {
        cacheFile(desc.filename, desc.data);
        files.insert(files.end(), std::move(desc));
    }
}
