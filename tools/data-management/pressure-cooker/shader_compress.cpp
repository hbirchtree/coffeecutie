#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/terminal/terminal_cursor.h>
#include <coffee/core/string_casting.h>
#include <coffee/core/plat/plat_environment.h>

#include <shaderc/shaderc.hpp>
#include <glslang/Public/ShaderLang.h>

#if defined(HAVE_SPIRVCROSS)
#include <spirv_glsl.hpp>
#endif

#define SHD_API "PressurizeShaders::"

using namespace CoffeePipeline;
using namespace Coffee;

static Vector<CString> shader_extensions = {
    "vert", "frag", "geom", "tesc", "tese",
    "comp"
};

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
    u32 version;
    bool es_sl;
};

struct CoffeeIncluder : shaderc::CompileOptions::IncluderInterface
{
    LinkList<shaderc_include_result> m_results;

public:
    virtual shaderc_include_result *GetInclude(
            const char *requested_source,
            shaderc_include_type type,
            const char * requesting_source,
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

            res.content = C_FCAST<cstring>(data.data);
            res.content_length = data.size;

            res.source_name = requested_source;
            res.source_name_length = StrLen(requested_source);

            res.user_data = r;

            return &res;
        }
        default:
            return nullptr;
        }
    }
    virtual void ReleaseInclude(shaderc_include_result *data)
    {
        auto pred = [&](shaderc_include_result& r)
        {
            if(&r == data)
            {
                delete C_RCAST<Resource*>(r.user_data);
                return true;
            }
            return false;
        };

        auto removeIt = std::remove_if(m_results.begin(), m_results.end(),
                                       pred);

        m_results.erase(removeIt, m_results.end());
    }
};

static Vector<u32> CompileSpirV(
        TerminalCursor& cursor,
        shaderc::Compiler& compiler,
        CString const& entrypoint,
        Bytes const& source,
        Path const& path,
        shaderc_optimization_level level,
        shaderc_target_env target_env = shaderc_target_env_opengl,
        GLSLTarget const& target = {450, false}
        )
{
    shaderc::CompileOptions options;

    options.SetIncluder(MkUq<CoffeeIncluder>());
    options.SetOptimizationLevel(
                level
                );
    options.SetSourceLanguage(shaderc_source_language_glsl);

    /* We might want to create Vulkan SPV in the future */
    options.SetTargetEnvironment(target_env, 0);

    /* We want to avoid setting locations manually */
    options.SetAutoMapLocations(true);

    shaderc_profile p =
            target.es_sl ? shaderc_profile_es
                         : shaderc_profile_core;

    if(!target.es_sl && target.version < 400)
        p = shaderc_profile_none;

    options.SetForcedVersionProfile(C_FCAST<i32>(target.version), p);

    auto type = shader_mapping[path.extension()];

    shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(
                C_RCAST<const char*>(source.data),
                source.size,
                type,
                path.internUrl.c_str(),
                entrypoint.c_str(),
                options
                );

    auto result = module.GetCompilationStatus();

    if(result != shaderc_compilation_status_success)
    {
        cursor.print(SHD_API
R"(Shader compilation failed ({1}) with:
{0}
Associated source:
{2})",
                     module.GetErrorMessage(),
                     path,
                     StrUtil::encapsulate(
                         C_RCAST<const char*>(source.data),
                         source.size)
                     );
        cursor.progress(SHD_API "Skipping shader");
        return {};
    }

    cursor.progress(SHD_API
                    "Shader compiled: GLSL:{1}:{2} {0}",
                    path.internUrl, target.version,
                    ((target.es_sl) ? "es" : "core")
                    );
    return {module.begin(), module.end()};
}

static void GenerateGLSL(TerminalCursor& cursor,
                         Vector<VirtFS::VirtDesc>& files,
                         Path const& srcFile,
                         Vector<u32> const& source,
                         GLSLTarget const& target)
{
#if defined(HAVE_SPIRVCROSS)
    spirv_cross::CompilerGLSL compiler(source.data(), source.size());

    spirv_cross::ShaderResources rsc = compiler.get_shader_resources();

    /* SPIRV-Cross does not recognize gl_BaseInstance */
    compiler.add_header_line("#define gl_BuiltIn_4425 gl_BaseInstance");

    if(target.es_sl && target.version == 100)
    {
        /* Remove "flat" qualifier always */
        compiler.add_header_line("#define flat");
        compiler.add_header_line("#define double float");

        /* Set some default precision, just in case */
        compiler.add_header_line("precision highp float;");
        compiler.add_header_line("precision mediump int;");

        /* Generate grid uniform for every array texture */
        for(auto const& img : rsc.sampled_images)
        {
            auto img_type = compiler.get_type_from_variable(img.id);

            if(img_type.image.dim == spv::Dim2D
                    && !img_type.image.arrayed)
                continue;

            compiler.add_header_line(
                        cStringFormat(
                            "uniform int {0}_GridSize;",
                            img.name
                            )
                        );
        }
    }

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
    opts.es = target.es_sl;

    /* When compiling for GLSL 4.10+, we always use SSO */
    if(!target.es_sl && target.version >= 410)
        opts.separate_shader_objects = true;

    compiler.set_common_options(opts);

    CString glsl;

    /* Compilation may fail for various reasons.
     *  We will report the error and not emit any GLSL. */
    try {
        glsl = compiler.compile();
        cursor.progress(SHD_API
                        "Shader code generated: GLSL:{0}:{1} {2}",
                        target.version, target.es_sl ? "es" : "core",
                        srcFile.internUrl);
    } catch(spirv_cross::CompilerError const& e) {
        cursor.print("{0}:{1}:{2}: {3}",
                     GetFileResourcePrefix() + "/" + srcFile.internUrl,
                     0,
                     Stacktracer::DemangleSymbol(typeid(e).name()),
                     e.what()
                     );
        return;
    }

    /* We take a round-trip to shaderc for validation
     *  of the generated GLSL */
    if(!target.es_sl)
    {
        shaderc::Compiler scompiler;
        auto data = CompileSpirV(
                    cursor, scompiler, srcFile.extension(),
                    Bytes::CreateString(glsl.c_str()), srcFile,
                    shaderc_optimization_level_zero,
                    shaderc_target_env_opengl, target
                    );

        /* If validation step fails, do not add the GLSL. It is invalid. */
        if(data.size() == 0)
            return;
    }

    Vector<u8> glslData(glsl.begin(), glsl.end());

    auto ext = srcFile.extension();
    auto path = srcFile.removeExt()
            .addExtension(opts.es ? "es" : "core")
            .addExtension(cast_pod(opts.version).c_str())
            .addExtension(ext.c_str());

    files.push_back({
                        path.internUrl.c_str(),
                        Bytes::CopyFrom(glslData),
                        VirtFS::File_Compressed
                    });
#endif
}

struct ShaderProcessor : FileProcessor
{
    virtual void process(Vector<VirtFS::VirtDesc> &files,
                         TerminalCursor &cursor);

    virtual void setBaseDirectories(const Vector<CString> &)
    {
    }
};

COFFAPI FileProcessor* CoffeeLoader()
{
    return new ShaderProcessor;
}

void ShaderProcessor::process(Vector<VirtFS::VirtDesc> &files,
                              TerminalCursor &cursor)
{
    shaderc::Compiler compiler;

    Map<Path, const VirtFS::VirtDesc*> selection;

    for(auto const& file : files)
    {
        Path p = Path::Mk(file.filename.c_str());

        auto it =std::find(
                    shader_extensions.begin(),
                    shader_extensions.end(),
                    p.extension());

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
                    compiler, path.first.extension(), path.second->data,
                    path.first, shaderc_optimization_level_performance
                    );

        if(optimized.size() == 0)
        {
            cursor.print("{0}:0: Failed to compile GLSL to SPIR-V",
                         (Path(GetFileResourcePrefix())
                          + path.first)
                         .internUrl
                         );
//            cursor.progress(SHD_API "Failed to compile GLSL to SPIR-V");
            continue;
        }

        Path binaryPath = path.first.addExtension("spv");

        newFiles.push_back({
                            binaryPath.internUrl.c_str(),
                            Bytes::CopyFrom(optimized),
                            VirtFS::File_Compressed
                        });

        /* Generate unoptimized SPIR-V binary for
         *  recompilation to GLSL */
        Vector<u32> unoptimized = CompileSpirV(
                    cursor,
                    compiler, path.first.extension(), path.second->data,
                    path.first, shaderc_optimization_level_zero,
                    shaderc_target_env_opengl
                    );

        if(unoptimized.size() == 0)
        {
            cursor.progress(SHD_API "Failed to create unoptimized binary");
            continue;
        }

        /* With the generated SPIR-V, cross-compile it to
         *  various GLSL/ES versions, for portability */

        GenerateGLSL(cursor, newFiles, path.first,
                     unoptimized, {100, true});
        GenerateGLSL(cursor, newFiles, path.first,
                     unoptimized, {300, true});

        GenerateGLSL(cursor, newFiles, path.first,
                     unoptimized, {330, false});
        GenerateGLSL(cursor, newFiles, path.first,
                     unoptimized, {430, false});
        GenerateGLSL(cursor, newFiles, path.first,
                     unoptimized, {460, false});
    }

    files.reserve(files.size() + newFiles.size());

    for(auto& desc : newFiles)
    {
        cacheFile(desc.filename, desc.data);
        files.insert(files.end(), std::move(desc));
    }
}
