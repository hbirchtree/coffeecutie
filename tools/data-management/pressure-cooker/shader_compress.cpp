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
            shaderc_include_type,
            const char * requesting_source,
            size_t)
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

    options.SetTargetEnvironment(target_env, 0);

    options.SetAutoMapLocations(true);
//    options.SetAutoBindUniforms(true);

    shaderc_profile p =
            target.es_sl ? shaderc_profile_es
                         : shaderc_profile_core;

    if(target.version < 400)
        p = shaderc_profile_none;

    options.SetForcedVersionProfile(C_FCAST<i32>(target.version), p);

    auto type = shader_mapping[path.extension()];

//    Path compFile = (Path(GetFileResourcePrefix()) + path);

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

    /* TODO: On GLSL ES 1.00, create uniforms for
     * - sampler2DArray
     * - sampler3D
     */

    spirv_cross::ShaderResources rsc = compiler.get_shader_resources();

    compiler.add_header_line("#define gl_BuiltIn_4425 gl_BaseInstance");

    if(target.es_sl && target.version == 100)
    {
        compiler.add_header_line("#define flat");
        compiler.add_header_line("#define uint int");
        compiler.add_header_line("#define double float");

        compiler.add_header_line("precision highp float;");
        compiler.add_header_line("precision mediump int;");

        compiler.add_header_line("#define sampler3D sampler2D");
        compiler.add_header_line("#define sampler2DArray sampler2D");

        compiler.add_header_line("#define gl_ClipDistance ClipDistance");
        compiler.add_header_line("#define gl_CullDistance CullDistance");

        compiler.add_header_line("#define gl_InstanceID InstanceID");
        compiler.add_header_line("uniform int InstanceID;");

//        compiler.add_header_line("vec4 texture2D(sampler2D tex, vec3 coord)"
//                                 "{"
//                                 "return texture2D(tex, coord.xy);"
//                                 "}");

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

    if(target.version < 460)
        compiler.add_header_line("#define gl_BaseInstance BaseInstance");


    if(target.version == 460)
    {
        compiler.add_header_line("#define gl_BaseInstanceARB"
                                 " gl_BaseInstance");
    }

    spirv_cross::CompilerGLSL::Options opts;
    opts.version = target.version;
    opts.es = target.es_sl;
    compiler.set_common_options(opts);

    CString glsl;
    try {
        glsl = compiler.compile();
    } catch(spirv_cross::CompilerError const& e) {
        cursor.print("{0}:{1}:{2}: {3}",
                     GetFileResourcePrefix() + "/" + srcFile.internUrl,
                     0,
                     Stacktracer::DemangleSymbol(typeid(e).name()),
                     e.what()
                     );
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

    shaderc::Compiler scompiler;

    if(!target.es_sl)
        CompileSpirV(cursor, scompiler, srcFile.extension(),
                     Bytes::CreateString(glsl.c_str()), srcFile,
                     shaderc_optimization_level_zero,
                     shaderc_target_env_opengl, target
                     );
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
