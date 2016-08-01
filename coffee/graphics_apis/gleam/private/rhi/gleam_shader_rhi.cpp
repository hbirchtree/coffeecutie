#include <coffee/graphics_apis/gleam/rhi/gleam_shader_rhi.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_api_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

bool GLEAM_Shader::compile(ShaderStage stage, Bytes &data)
{
    CString str_s((cstring)data.data,data.size);
    cstring str = str_s.c_str();

    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
    {
        CGL33::ShaderAlloc(1,stage,&m_handle);
        CGL33::ShaderSource(m_handle,1,&str);
        bool stat = CGL33::ShaderCompile(m_handle);

        if(GL_DEBUG_MODE && !stat)
        {
            CString log = CGL33::ShaderGetLog(m_handle);
            cDebug("Shader compilation error: {0}",log);
            return false;
        }

        m_stages = stage;

        return stat;
    }else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        m_handle = CGL43::ProgramCreate(stage,1,&str);

        if(GL_DEBUG_MODE && m_handle == 0)
        {
            CString log = CGL43::ProgramGetLog(m_handle);
            cDebug("Shader program compilation error: {0}",log);
            return false;
        }

        m_stages = stage;

        return m_handle!=0;
    }else
        return false;
}

void GLEAM_Shader::dealloc()
{
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
    {
        if(m_handle!=0)
            CGL33::ShaderFree(1,&m_handle);
    }
    else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        if(m_handle!=0)
            CGL43::ProgramFree(1,&m_handle);
    }
}

bool GLEAM_Pipeline::attach(const GLEAM_Shader &shader, const ShaderStage &stages)
{
    if(shader.m_handle==0)
        return false;
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
    {
        if(m_handle==0)
            CGL33::ProgramAlloc(1,&m_handle);
        CGL33::ShaderAttach(m_handle,shader.m_handle);
        return true;
    }else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
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
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
    {
        bool stat = CGL33::ProgramLink(m_handle);
        if(GL_DEBUG_MODE && !stat)
        {
            CString log = CGL33::ProgramGetLog(m_handle);
            cDebug("Program link error: {0}",log);
        }
        return stat;
    }
    else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        bool stat = CGL43::PipelineValidate(m_handle);
        if(GL_DEBUG_MODE && !stat)
        {
            ShaderStage s = CGL43::PipelineGetStages(m_handle);
            CString log = CGL43::PipelineGetLog(m_handle);
            cDebug("Pipeline validation error: {0}",log);
            cDebug("Pipeline stages: {0}",(uint32)s);
        }
        return stat;
    }
    return false;
}

void GLEAM_Pipeline::bind()
{
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
        CGL33::ProgramUse(m_handle);
    else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
        CGL43::PipelineBind(m_handle);
}

void GLEAM_Pipeline::unbind()
{
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
        CGL33::ProgramUse(0);
    else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
        CGL43::PipelineBind(0);
}

bool GLEAM_ShaderUniformState::setUniform(const GLEAM_UniformDescriptor &value,
                                          GLEAM_UniformValue *data)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    data->flags = value.m_flags;
    m_uniforms[idx] = data;
    return true;
}

bool GLEAM_ShaderUniformState::setSampler(const GLEAM_UniformDescriptor &value,
                                          const GLEAM_SamplerHandle &sampler)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    m_samplers[idx] = &sampler;
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
    if(GL_CURR_API==GL_3_3 || GL_CURR_API == GLES_3_0 || GL_CURR_API == GLES_3_2)
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
                uniforms->push_back({});
                GLEAM_UniformDescriptor &desc = uniforms->back();
                desc.m_flags = 0;

                desc.m_name = unif_names[i];
                desc.m_idx = CGL33::ProgramUnifGetLoc(prog,desc.m_name.c_str());
                desc.stages = ShaderStage::All;
                switch(unif_types[i])
                {
                case GL_SAMPLER_2D:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::Sam2D;
                    break;
                case GL_SAMPLER_3D:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::Sam3D;
                    break;
                case GL_SAMPLER_2D_ARRAY:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::Sam2DA;
                    break;
                case GL_SAMPLER_CUBE:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::SamCube;
                    break;
                case GL_SAMPLER_CUBE_MAP_ARRAY:
                    desc.m_flags = GLEAM_API::SamplerT|GLEAM_API::SamCubeA;
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

#ifdef COFFEE_GLEAM_DESKTOP
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
#endif

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

                case GL_FLOAT_MAT2:
                    desc.m_flags = GLEAM_API::ScalarT|GLEAM_API::Mat2T;
                    break;
                case GL_FLOAT_MAT3:
                    desc.m_flags = GLEAM_API::ScalarT|GLEAM_API::Mat3T;
                    break;
                case GL_FLOAT_MAT4:
                    desc.m_flags = GLEAM_API::ScalarT|GLEAM_API::Mat4T;
                    break;

                default:
                    cDebug("Unhandled shader type: {0}, uniform={1}",
                           unif_types[i],unif_names[i]);
                }
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
