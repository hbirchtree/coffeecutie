#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Shader : GraphicsAPI::Shader
{
    friend struct GLEAM_API;
    friend struct GLEAM_Pipeline;
    friend void GetShaderUniforms(
            const GLEAM_Pipeline &pipeline,
            Vector<GLEAM_UniformDescriptor> *uniforms,
            Vector<GLEAM_ProgramParameter> *params,
            Vector<GLEAM_ProgramParameter> *outputs);

    GLEAM_Shader():
        Shader(0),
        m_handle(0)
    {
    }

    /*!
     * \brief Compiles a shader for the given stage, setting the handle
     *  to the object and returning status on success.
     *
     * The given shader data does not need to be null-terminated, as the
     *  given size is used.
     *
     * On OpenGL ES 2.0, this function will attempt to transform a
     *  GLSL 3.30 shader into functioning GLSL ES 1.00 using a set
     *  of substitutions, adding a couple of functions
     *  (`texture` => `texture2D`, `texture2DArray` => wrapper around `texture2D`)
     *
     * This will work for simple shaders, but advanced shaders will still
     *  need a rewrite to work on this.
     *
     * BEWARE: If you are on GLSL ES 1.00 and use sampler2DArray emulation,
     *  you will have to specify a uniform `${SAMPLERNAME}_gridSize`
     *  which is the `ceil(sqrt())` of the texture array's size. This is
     *  necessary for the sampling function to work.
     *
     * Another note is that we are abusing the name texture2DArray
     *  for sampler2DArray sampling. Sampling using
     * \code{.cpp}
     *  vec4 sample = texture2DArray(samplerHandle, vec3(...));
     * \endcode
     *
     * Other than that, `in` and `out` are substituted for `attribute` and `varying` for vertex shaders, `in` becomes varying in fragment shaders.
     *
     * \param stage Which shader stage to compile
     * \param data A container with GLSL source code, null-terminated
     * \return true upon success
     */
    bool compile(ShaderStage stage, Bytes const& data);
    void dealloc();

protected:

    CGhnd m_handle;
    ShaderStage m_stages;

public:
    CGhnd internalHandle() const
    {
        return m_handle;
    }
};

struct GLEAM_Pipeline : GraphicsAPI::Pipeline
{
    friend struct GLEAM_PipelineDumper;
    friend struct GLEAM_API;
    friend void GetShaderUniforms(
            const GLEAM_Pipeline &pipeline,
            Vector<GLEAM_UniformDescriptor> *uniforms,
            Vector<GLEAM_ProgramParameter> *params,
            Vector<GLEAM_ProgramParameter> *outputs);

    GLEAM_Pipeline():
        Pipeline(0),
        m_handle(0)
    {
    }

    bool attach(GLEAM_Shader const&shader, ShaderStage const& stages);
    GLEAM_Shader &storeShader(GLEAM_Shader&& shader);

    bool assemble();

    void bind() const;
    void unbind() const;

    void dealloc();

protected:
    CGhnd m_handle;

    struct shader_cntainer
    {
        GLEAM_Shader const* shader;
        ShaderStage stages;
    };

    Vector<shader_cntainer> m_programs;
    LinkList<GLEAM_Shader> m_ownedPrograms;

public:
    CGhnd pipelineHandle() const
    {
        return m_handle;
    }
    Vector<shader_cntainer> const& internalHandles() const
    {
        return m_programs;
    }
};

struct GLEAM_ProgramParameter : GraphicsAPI::ProgramParameter
{
    ShaderStage stages;
};

struct GLEAM_UniformDescriptor : GraphicsAPI::UniformDescriptor
{
    ShaderStage stages;
};

struct GLEAM_UniformValue : GraphicsAPI::UniformValue
{
};

struct GLEAM_BufferSection
{
    szptr offset;
    szptr size;
};

struct GLEAM_ShaderUniformState : GraphicsAPI::ShaderUniformState
{
    friend struct GLEAM_API;

    bool setUniform(GLEAM_UniformDescriptor const& value, GLEAM_UniformValue* data);
    bool setSampler(GLEAM_UniformDescriptor const& value,GLEAM_SamplerHandle const* sampler);
    bool setImage(GLEAM_UniformValue const& value, GLEAM_ShaderImage* img);

    bool setUBuffer(GLEAM_UniformDescriptor const& value,
                    GLEAM_UniformBuffer const* buf,
                    GLEAM_BufferSection const& sec);
    bool setSBuffer(GLEAM_UniformDescriptor const& value,
                    GLEAM_ShaderBuffer const* buf,
                    GLEAM_BufferSection const& sec);

    void clear();

protected:

    template<typename T>
    struct buffer_container
    {
        GLEAM_BufferSection sec;
        T const* buff;
        ShaderStage stages;
    };

    template<typename T>
    struct uniform_container
    {
        T const* value;
        ShaderStage stages;
    };

    Map<uint32,uniform_container<GLEAM_UniformValue>> m_uniforms;
    Map<uint32,uniform_container<GLEAM_SamplerHandle>> m_samplers;
    Map<uint32,buffer_container<GLEAM_UniformBuffer>> m_ubuffers;
    Map<uint32,buffer_container<GLEAM_ShaderBuffer>> m_sbuffers;
};

struct GLEAM_PipelineDumper : GraphicsProfiler::PipelineDumper<GLEAM_Pipeline>
{
    GLEAM_PipelineDumper(GLEAM_Pipeline& pipeline);
    void dump(cstring out);
};

/*!
 * \brief Retrieve shader information, such as uniforms, uniform buffers, SSBOs, atomic counters etc.
 * If specified, vertex attributes can also be retrieved.
 * \param pipeline
 * \param uniforms
 * \param params
 */
extern void GetShaderUniforms(
        GLEAM_Pipeline const& pipeline,
        C_OPTIONAL Vector<GLEAM_UniformDescriptor>* uniforms,
        C_OPTIONAL Vector<GLEAM_ProgramParameter> *params,
        C_OPTIONAL Vector<GLEAM_ProgramParameter> *outputs);

}
}
}
