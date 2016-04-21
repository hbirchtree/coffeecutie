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

void GetShaderUniforms(const GLEAM_Pipeline &pipeline, Vector<GLEAM_UniformDescriptor> *uniforms)
{
    if(GL_CURR_API==GL_3_3)
    {
        CGhnd prog = pipeline.m_handle;

        /* Get typical uniforms */
        {
            uint32 num_uniforms;
            cstring_w* unif_names;
            uint32* unif_types;
            int32* unif_sizes;
            CGL33::ProgramUnifGet(prog,&num_uniforms,&unif_names,
                                  &unif_types,&unif_sizes);
            if(num_uniforms==0)
                return;
            uniforms->reserve(num_uniforms);
            for(uint32 i=0;i<num_uniforms;i++)
            {
                GLEAM_UniformDescriptor desc;
                desc.m_name = unif_names[i];
                desc.m_idx = CGL33::ProgramUnifGetLoc(prog,desc.m_name.c_str());
                desc.stages = ShaderStage::All;
                switch(unif_types[i])
                {
                case GL_SAMPLER_2D:
                    desc.m_flags = GLEAM_API::SamplerT + GLEAM_API::Sam2D;
                    break;
                case GL_SAMPLER_3D:
                    desc.m_flags = GLEAM_API::SamplerT + GLEAM_API::Sam3D;
                    break;
                case GL_SAMPLER_2D_ARRAY:
                    desc.m_flags = GLEAM_API::SamplerT + GLEAM_API::Sam2DA;
                    break;
                case GL_SAMPLER_CUBE:
                    desc.m_flags = GLEAM_API::SamplerT + GLEAM_API::SamCube;
                    break;
                case GL_SAMPLER_CUBE_MAP_ARRAY:
                    desc.m_flags = GLEAM_API::SamplerT + GLEAM_API::SamCubeA;
                    break;

                case GL_UNSIGNED_INT_SAMPLER_2D:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::UIntegerT|GLEAM_API::Sam2D;
                    break;
                case GL_UNSIGNED_INT_SAMPLER_3D:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::UIntegerT|GLEAM_API::Sam3D;
                    break;
                case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::UIntegerT|GLEAM_API::Sam2DA;
                    break;
                case GL_UNSIGNED_INT_SAMPLER_CUBE:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::UIntegerT|GLEAM_API::SamCube;
                    break;
                case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::UIntegerT|GLEAM_API::SamCubeA;
                    break;

                case GL_INT_SAMPLER_2D:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::IntegerT|GLEAM_API::Sam2D;
                    break;
                case GL_INT_SAMPLER_3D:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::IntegerT|GLEAM_API::Sam3D;
                    break;
                case GL_INT_SAMPLER_2D_ARRAY:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::IntegerT|GLEAM_API::Sam2DA;
                    break;
                case GL_INT_SAMPLER_CUBE:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::IntegerT|GLEAM_API::SamCube;
                    break;
                case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::IntegerT|GLEAM_API::SamCubeA;
                    break;

                case GL_FLOAT:
                    desc.m_flags = GLEAM_API::ScalarT;
                    break;
                case GL_FLOAT_VEC2:
                    desc.m_flags = GLEAM_API::ScalarT|GLEAM_API::Vec2T;
                    break;
                case GL_FLOAT_VEC3:
                    desc.m_flags = GLEAM_API::ScalarT|GLEAM_API::Vec3T;
                    break;
                case GL_FLOAT_VEC4:
                    desc.m_flags = GLEAM_API::ScalarT|GLEAM_API::Vec4T;
                    break;

                case GL_DOUBLE:
                    desc.m_flags = GLEAM_API::BigScalarT;
                    break;
                case GL_DOUBLE_VEC2:
                    desc.m_flags = GLEAM_API::BigScalarT|GLEAM_API::Vec2T;
                    break;
                case GL_DOUBLE_VEC3:
                    desc.m_flags = GLEAM_API::BigScalarT|GLEAM_API::Vec3T;
                    break;
                case GL_DOUBLE_VEC4:
                    desc.m_flags = GLEAM_API::BigScalarT|GLEAM_API::Vec4T;
                    break;

                case GL_UNSIGNED_INT:
                    desc.m_flags = GLEAM_API::UIntegerT;
                    break;
                case GL_UNSIGNED_INT_VEC2:
                    desc.m_flags = GLEAM_API::UIntegerT|GLEAM_API::Vec2T;
                    break;
                case GL_UNSIGNED_INT_VEC3:
                    desc.m_flags = GLEAM_API::UIntegerT|GLEAM_API::Vec3T;
                    break;
                case GL_UNSIGNED_INT_VEC4:
                    desc.m_flags = GLEAM_API::UIntegerT|GLEAM_API::Vec4T;
                    break;

                case GL_INT:
                    desc.m_flags = GLEAM_API::IntegerT;
                    break;
                case GL_INT_VEC2:
                    desc.m_flags = GLEAM_API::IntegerT|GLEAM_API::Vec2T;
                    break;
                case GL_INT_VEC3:
                    desc.m_flags = GLEAM_API::IntegerT|GLEAM_API::Vec3T;
                    break;
                case GL_INT_VEC4:
                    desc.m_flags = GLEAM_API::IntegerT|GLEAM_API::Vec4T;
                    break;
                }
                uniforms->push_back(desc);
                delete[] unif_names[i];
            }
            delete[] unif_names;
            delete[] unif_types;
            delete[] unif_sizes;
        }
        /* Get uniforms buffers */
        {
        }
    }else if(GL_CURR_API==GL_4_3){

    }
}

}
}
}
