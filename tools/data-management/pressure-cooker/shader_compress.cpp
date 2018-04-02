#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/terminal/terminal_cursor.h>
#include <coffee/core/string_casting.h>

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

static Vector<u32> CompileSpirV(
        TerminalCursor& cursor,
        shaderc::Compiler& compiler,
        Bytes const& source,
        Path const& path,
        shaderc_optimization_level level
        )
{
    shaderc::CompileOptions options;

    options.SetOptimizationLevel(
                level
                );
    options.SetSourceLanguage(shaderc_source_language_glsl);
    options.SetTargetEnvironment(shaderc_target_env_opengl, 0);

    auto type = shader_mapping[path.extension()];

    shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(
                C_RCAST<const char*>(source.data),
                source.size, type,
            path.internUrl.c_str(),
            options
            );

    auto result = module.GetCompilationStatus();

    if(result != shaderc_compilation_status_success)
    {
        cursor.print(SHD_API "Shader compilation failed ({1}) with:\n"
                             "{0}",
                     module.GetErrorMessage(),
                     path);
        cursor.progress(SHD_API "Skipping shader");
        return {};
    }

    cursor.progress(SHD_API "Shader compiled: {0}", path);
    return {module.begin(), module.end()};
}

static bool ValidateGLSL(CString shader)
{
    using namespace glslang;

    TShader shaderContainer(EShLangVertex);

    const char* shader_str = shader.c_str();
    shaderContainer.setStrings(&shader_str, 1);

    shaderContainer.setEnvClient(EShClientOpenGL,
                                 EShTargetOpenGL_450);

    shaderContainer.setEnvTarget(EShTargetSpv, EShTargetSpv_1_0);

    TBuiltInResource resource = {};
    return shaderContainer.parse(&resource, 100, EEsProfile,
                                 true, false, {});
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

    if(target.es_sl && target.version == 100)
    {
        compiler.add_header_line("#define uint int");
        compiler.add_header_line("#define double float");

        compiler.add_header_line("#define sampler3D sampler2D");
        compiler.add_header_line("#define sampler2DArray sampler2D");

        compiler.add_header_line("#define gl_ClipDistance ClipDistance");
        compiler.add_header_line("#define gl_CullDistance CullDistance");

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

    spirv_cross::CompilerGLSL::Options opts;
    opts.version = target.version;
    opts.es = target.es_sl;
    compiler.set_common_options(opts);

    CString glsl = compiler.compile();

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

//    if(target.version == 100 || target.version == 330)
//        cDebug("\n{0}", glsl);
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

    for(auto const& path : selection)
    {
        /* Generate optimized SPIR-V binary */
        Vector<u32> optimized = CompileSpirV(
                    cursor,
                    compiler, path.second->data,
                    path.first, shaderc_optimization_level_performance
                    );

        if(optimized.size() == 0)
        {
            cursor.progress(SHD_API "Failed to compile GLSL to SPIR-V");
            continue;
        }

        Path binaryPath = path.first.addExtension("spv");

        files.push_back({
                            binaryPath.internUrl.c_str(),
                            Bytes::CopyFrom(optimized),
                            VirtFS::File_Compressed
                        });

        /* Generate unoptimized SPIR-V binary for
         *  recompilation to GLSL */
        Vector<u32> unoptimized = CompileSpirV(
                    cursor,
                    compiler, path.second->data,
                    path.first, shaderc_optimization_level_zero
                    );

        if(unoptimized.size() == 0)
        {
            cursor.progress(SHD_API "Failed to create unoptimized binary");
            continue;
        }

        /* With the generated SPIR-V, cross-compile it to
         *  various GLSL/ES versions, for portability */

        GenerateGLSL(cursor, files, path.first, unoptimized, {100, true});
        GenerateGLSL(cursor, files, path.first, unoptimized, {300, true});

        GenerateGLSL(cursor, files, path.first, unoptimized, {330, false});
        GenerateGLSL(cursor, files, path.first, unoptimized, {430, false});
    }
}
