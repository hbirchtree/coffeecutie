#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Shader : GraphicsAPI::Shader
{
    friend struct GLEAM_API;
    friend struct GLEAM_Pipeline;
    friend void GetShaderUniforms(const GLEAM_Pipeline &pipeline,
                                  Vector<GLEAM_UniformDescriptor> *uniforms,
                                  Vector<GLEAM_ProgramParameter> *params);

    GLEAM_Shader():
        Shader(0),
        m_handle(0)
    {
    }

    bool compile(ShaderStage stage, Bytes const& data);
    void dealloc();

protected:

    CGhnd m_handle;
    ShaderStage m_stages;
};

struct GLEAM_Pipeline : GraphicsAPI::Pipeline
{
    friend struct GLEAM_PipelineDumper;
    friend struct GLEAM_API;
    friend void GetShaderUniforms(const GLEAM_Pipeline &pipeline,
                                  Vector<GLEAM_UniformDescriptor> *uniforms,
                                  Vector<GLEAM_ProgramParameter> *params);

    GLEAM_Pipeline():
        Pipeline(0),
        m_handle(0)
    {
    }

    bool attach(GLEAM_Shader const&shader, ShaderStage const& stages);

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
};

struct GLEAM_ProgramParameter : GraphicsAPI::ProgramParameter
{
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

extern void GetShaderUniforms(GLEAM_Pipeline const& pipeline,
                              Vector<GLEAM_UniformDescriptor>* uniforms,
                              Vector<GLEAM_ProgramParameter> *params);

}
}
}
