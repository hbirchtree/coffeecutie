#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Shader : GraphicsAPI::Shader
{
    friend struct GLEAM_API;
    friend struct GLEAM_Pipeline;

    GLEAM_Shader():
        Shader(0),
        m_handle(0)
    {
    }

    bool compile(ShaderStage stage,cstring str);

protected:
    void dealloc();

    CGhnd m_handle;
    ShaderStage m_stages;
};

struct GLEAM_Pipeline : GraphicsAPI::Pipeline
{
    friend struct GLEAM_API;

    GLEAM_Pipeline():
        Pipeline(0),
        m_handle(0)
    {
    }

    bool attach(GLEAM_Shader const&shader, ShaderStage const& stages);

    bool assemble();

    void bind();
    void unbind();
protected:
    CGhnd m_handle;

    struct shader_cntainer
    {
        GLEAM_Shader const* shader;
        ShaderStage stages;
    };

    Vector<shader_cntainer> m_programs;
};

struct GLEAM_UniformDescriptor : GraphicsAPI::UniformDescriptor
{
};

struct GLEAM_UniformValue
{
    CByteData const* data;
    uint32 flags;
};

struct GLEAM_BufferSection
{
    szptr offset;
    szptr size;
};

struct GLEAM_ShaderUniformState : GraphicsAPI::ShaderUniformState
{
    friend struct GLEAM_API;

    bool setUniform(GLEAM_UniformDescriptor const& value, GLEAM_UniformValue const* data);
    bool setSampler(GLEAM_UniformDescriptor const& value,GLEAM_SamplerHandle const& sampler);

    bool setUBuffer(GLEAM_UniformDescriptor const& value,
                    GLEAM_UniformBuffer const* buf,
                    GLEAM_BufferSection const& sec);
    bool setSBuffer(GLEAM_UniformDescriptor const& value,
                    GLEAM_ShaderBuffer const* buf,
                    GLEAM_BufferSection const& sec);

protected:

    template<typename T>
    struct buffer_container
    {
        GLEAM_BufferSection sec;
        T const* buff;
    };

    Map<uint32,GLEAM_UniformValue const*> m_uniforms;
    Map<uint32,GLEAM_SamplerHandle> m_samplers;
    Map<uint32,buffer_container<GLEAM_UniformBuffer>> m_ubuffers;
    Map<uint32,buffer_container<GLEAM_ShaderBuffer>> m_sbuffers;
};

}
}
}
