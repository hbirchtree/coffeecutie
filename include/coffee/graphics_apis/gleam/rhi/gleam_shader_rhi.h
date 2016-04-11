#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Shader : GraphicsAPI::Shader
{
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
    GLEAM_Pipeline():
        Pipeline(0),
        m_handle(0)
    {
    }

    bool attach(GLEAM_Shader const&shader);

    bool assemble();

    void bind();
    void unbind();
protected:
    CGhnd m_handle;
};

struct GLEAM_UniformDescriptor : GraphicsAPI::UniformDescriptor
{
};

struct GLEAM_ShaderUniformState : GraphicsAPI::ShaderUniformState
{
    friend struct GLEAM_API;

    bool setUniform(GLEAM_UniformDescriptor const& value, CByteData const* data);
    bool setSampler(GLEAM_UniformDescriptor const& value, GLEAM_SamplerHandle const& sampler);
    bool setUBuffer(GLEAM_UniformDescriptor const& value, GLEAM_UniformBuffer const* buf);
    bool setSBuffer(GLEAM_UniformDescriptor const& value, GLEAM_ShaderBuffer const* buf);

protected:
    Map<uint32,CByteData const*> m_uniforms;
    Map<uint32,GLEAM_SamplerHandle> m_samplers;
    Map<uint32,GLEAM_UniformBuffer const*> m_ubuffers;
    Map<uint32,GLEAM_ShaderBuffer const*> m_sbuffers;
};

}
}
}
