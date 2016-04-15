#include <coffee/graphics_apis/gleam/rhi/gleam_shader_rhi.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_api_rhi.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

bool GLEAM_Shader::compile(ShaderStage stage, cstring str)
{
    if(GL_CURR_API==GL_3_3)
    {
        CGL33::ShaderAlloc(1,stage,&m_handle);
        CGL33::ShaderSource(m_handle,1,&str);
        return CGL33::ShaderCompile(m_handle);
    }else if(GL_CURR_API==GL_4_3)
    {
        m_handle = CGL43::ProgramCreate(stage,1,&str);
        return m_handle!=0;
    }else
        return false;
}

void GLEAM_Shader::dealloc()
{
    if(GL_CURR_API==GL_3_3)
    {
        if(m_handle!=0)
            CGL33::ShaderFree(1,&m_handle);
    }
    else if(GL_CURR_API==GL_4_3)
    {
        if(m_handle!=0)
            CGL43::ProgramFree(1,&m_handle);
    }
}

bool GLEAM_Pipeline::attach(const GLEAM_Shader &shader, const ShaderStage &stages)
{
    if(shader.m_handle==0)
        return false;
    if(GL_CURR_API==GL_3_3)
    {
        if(m_handle==0)
            CGL33::ProgramAlloc(1,&m_handle);
        CGL33::ShaderAttach(m_handle,shader.m_handle);
        return true;
    }else if(GL_CURR_API==GL_4_3)
    {
        if(m_handle==0)
            CGL43::PipelineAlloc(1,&m_handle);
        bool stat = CGL43::PipelineUseStages(m_handle,shader.m_stages&stages,shader.m_handle);
        if(stat)
            m_programs.push_back({&shader,shader.m_stages&stages});
        return stat;
    }
    return false;
}

bool GLEAM_Pipeline::assemble()
{
    if(m_handle==0)
        return false;
    if(GL_CURR_API==GL_3_3)
    {
        return CGL33::ProgramLink(m_handle);
    }
    else if(GL_CURR_API==GL_4_3)
    {
        return CGL43::PipelineValidate(m_handle);
    }
    return false;
}

void GLEAM_Pipeline::bind()
{
    if(GL_CURR_API==GL_3_3)
        CGL33::ProgramUse(m_handle);
    else if(GL_CURR_API==GL_4_3)
        CGL43::PipelineBind(m_handle);
}

void GLEAM_Pipeline::unbind()
{
    if(GL_CURR_API==GL_3_3)
        CGL33::ProgramUse(0);
    else if(GL_CURR_API==GL_4_3)
        CGL43::PipelineBind(0);
}

bool GLEAM_ShaderUniformState::setUniform(const GLEAM_UniformDescriptor &value,
                                          const GLEAM_UniformValue *data)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    m_uniforms[idx] = data;
    return true;
}

bool GLEAM_ShaderUniformState::setSampler(const GLEAM_UniformDescriptor &value,
                                          const GLEAM_SamplerHandle &sampler)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    m_samplers[idx] = sampler;
    return true;
}

bool GLEAM_ShaderUniformState::setUBuffer(const GLEAM_UniformDescriptor &value,
                                          const GLEAM_UniformBuffer *buf,
                                          GLEAM_BufferSection const& sec)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    m_ubuffers[idx] = {sec,buf};
    return true;
}

bool GLEAM_ShaderUniformState::setSBuffer(const GLEAM_UniformDescriptor &value,
                                          const GLEAM_ShaderBuffer *buf,
                                          GLEAM_BufferSection const& sec)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    m_sbuffers[idx] = {sec,buf};
    return true;
}

}
}
}
