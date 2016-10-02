#include <coffee/graphics_apis/gleam/rhi/gleam_shader_rhi.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_api_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

bool GLEAM_Shader::compile(ShaderStage stage, Bytes &data)
{
    if(GL_CURR_API==GL_3_3 || GL_CURR_API==GLES_3_0)
    {
        int32 slen = data.size;
        CGL33::ShaderAlloc(1,stage,&m_handle);
        CGL33::ShaderSource(m_handle,1,&slen,(cstring*)&data.data);
        bool stat = CGL33::ShaderCompile(m_handle);

        if(GL_DEBUG_MODE && !stat)
        {
            CString log = CGL33::ShaderGetLog(m_handle);
            cWarning("Shader compilation error: {0}",log);
            return false;
        }

        m_stages = stage;

        return stat;
    }else if(GL_CURR_API==GL_4_3 || GL_CURR_API==GLES_3_2)
    {
        cstring str = (cstring)data.data;
        m_handle = CGL43::ProgramCreate(stage,1,&str);

        if(GL_DEBUG_MODE && m_handle == 0)
        {
            CString log = CGL43::ProgramGetLog(m_handle);
            cWarning("Shader program compilation error: {0}",log);
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
    m_uniforms[idx] = {data,value.stages};
    return true;
}

bool GLEAM_ShaderUniformState::setSampler(const GLEAM_UniformDescriptor &value,
                                          const GLEAM_SamplerHandle *sampler)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    m_samplers[idx] = {sampler,value.stages};
    return true;
}

bool GLEAM_ShaderUniformState::setUBuffer(const GLEAM_UniformDescriptor &value,
                                          const GLEAM_UniformBuffer *buf,
                                          GLEAM_BufferSection const& sec)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    m_ubuffers[idx] = {sec,buf,value.stages};
    return true;
}

bool GLEAM_ShaderUniformState::setSBuffer(const GLEAM_UniformDescriptor &value,
                                          const GLEAM_ShaderBuffer *buf,
                                          GLEAM_BufferSection const& sec)
{
    if(value.m_idx<0)
        return false;
    uint32 idx = value.m_idx;
    m_sbuffers[idx] = {sec,buf,value.stages};
    return true;
}

void GetShaderUniforms(const GLEAM_Pipeline &pipeline,
                       Vector<GLEAM_UniformDescriptor> *uniforms,
                       Vector<GLEAM_ProgramParameter> *params)
{
    using namespace ShaderTypes;

    if(GL_CURR_API==GL_3_3 || GL_CURR_API == GLES_3_0 || GL_CURR_API == GLES_3_2)
    {
        /* Does not differentiate between shader stages and their uniforms */
        /* GL 4.3+ do not work with this */
        CGhnd prog = pipeline.m_handle;

        /* Get typical uniforms */
        {
            uint32 num_uniforms;
            CGL33::UnifValInfo* unifs;
            CGL33::ProgramUnifGet(prog,&num_uniforms,&unifs);
            if(num_uniforms==0)
                return;
            uniforms->reserve(num_uniforms);
            for(uint32 i=0;i<num_uniforms;i++)
            {
                auto const& v = unifs[i];

                uniforms->push_back({});
                GLEAM_UniformDescriptor &desc = uniforms->back();
                desc.m_flags = 0;

                desc.m_name = v.name;
                desc.m_idx = CGL33::ProgramUnifGetLoc(prog,desc.m_name.c_str());
                desc.stages = ShaderStage::All;
                desc.m_flags = to_enum_shtype(v.type);
                delete[] v.name;
            }
            delete[] unifs;
        }
        /* Get uniforms buffers */
        {
        }
    }else if(GL_CURR_API==GL_4_3){

        enum GL_PROP_IDX
        {
            G_NameLen,
            G_Type,
            G_Loc,
            G_ArrStride,
            G_ArrSize,
            G_BlkIdx,
            G_Offset,
        };

        for(auto& p : pipeline.m_programs)
        {
            int32 num_unifs;
            const CGhnd& hnd = p.shader->m_handle;

            /* Get number of uniform variables */
            glGetProgramInterfaceiv(hnd, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_unifs);

            for(int32 i=0;i<num_unifs;i++)
            {
                /* Per uniform, acquire the following variables */
                const CGenum props_to_get[] = {
                    /* General data */
                    GL_NAME_LENGTH,
                    GL_TYPE,
                    GL_LOCATION,

                    /* Array data */
                    GL_ARRAY_STRIDE,
                    GL_ARRAY_SIZE,

                    /* Block data */
                    GL_BLOCK_INDEX,
                    GL_OFFSET,
                };
                int32 props_out[sizeof(props_to_get)/sizeof(CGenum)];
                glGetProgramResourceiv(hnd, GL_UNIFORM, i,
                                       sizeof(props_to_get)/sizeof(CGenum),
                                       props_to_get, sizeof(props_out), nullptr,
                                       props_out);

                /* Create the uniform value */
                uniforms->push_back({});
                GLEAM_UniformDescriptor& desc = uniforms->back();
                desc.stages = p.stages;

                desc.m_flags = to_enum_shtype(props_out[G_Type]);
                desc.m_idx = props_out[G_Loc];

                if((desc.m_blkIdx = props_out[G_BlkIdx]) != -1)
                {
                    desc.m_blkIdx = props_out[G_BlkIdx];
                    desc.m_offset = props_out[G_Offset];
                }
                if(props_out[G_ArrSize] != -1)
                {
                    desc.m_arrSize = props_out[G_ArrSize];
                    desc.m_arrStride = props_out[G_ArrStride];
                }

                /* Get the uniform name */
                desc.m_name.resize(props_out[G_NameLen]);
                glGetProgramResourceName(hnd, GL_UNIFORM, i,
                                         props_out[G_NameLen],
                                         nullptr,&desc.m_name[0]);
                /* Remove the null character */
                if(desc.m_name.size() > 1)
                    desc.m_name.resize(desc.m_name.size()-1);
            }
        }
    }
}

GLEAM_PipelineDumper::GLEAM_PipelineDumper(GLEAM_Pipeline &pipeline):
    PipelineDumper(pipeline)
{
}

void GLEAM_PipelineDumper::dump(cstring out)
{
    int32* bin_fmts = &GLEAM_API::instance_data->GL_CACHED
            .NUM_PROGRAM_BINARY_FORMATS;
    if(*bin_fmts == -1)
    {
        /**/
        CGL43::Debug::GetIntegerv(
                    GL_NUM_PROGRAM_BINARY_FORMATS,
                    bin_fmts);
    }
    if(*bin_fmts <= 0)
    {
        cVerbose(6,"No GL program binary formats supported");
        return;
    }
    if(CGL43::GetProgramBinarySupported() &&
            (GL_CURR_API == GL_3_3
             || GL_CURR_API == GLES_3_0))
    {
        /* Just dump the program binary, nothing else is needed */
        CResources::Resource output(
                    out,
                    ResourceAccess::NewFile);

        Vector<byte_t> program_data;
        /* We'll fit the binary type in here */
        program_data.resize(sizeof(GL_CURR_API)
                            +sizeof(CGenum));
        CGenum t = GL_NONE;
        cVerbose(6,"About to get program binary");
        /* Append program data */
        if(!CGL43::ProgramGetBinary(m_pipeline.m_handle,
                                    &t,
                                    &program_data))
            return;
        cVerbose(6,"Acquired program binary");
        /* Insert API version */
        MemCpy(&program_data[0],
                &GL_CURR_API,
                sizeof(GL_CURR_API));
        /* Put binary type in there */
        MemCpy(&program_data[sizeof(GL_CURR_API)],
                &t,
                sizeof(t));

        /* Create the output resource */
        output.size = program_data.size();
        output.data = program_data.data();
        if(CResources::FileCommit(output))
            cVerbose(5,"Dumped program ({0}) into file {1}",
                     m_pipeline.m_handle,
                     output.resource());
        else
            cVerbose(5,"Dumping program ({0}) into file {1} failed",
                     m_pipeline.m_handle,
                     output.resource());
    }else if(GL_CURR_API == GL_4_3
             || GL_CURR_API == GLES_3_2)
    {
        /* With pipeline objects we must fetch several
         *  shader programs and dump these */
        /* We also need some struct to tell us offsets of
         *  these programs and what shaders they are */
        struct GL_DUMPED_FMT
        {
            uint64 offset;
            ShaderStage stg;
        };
        struct GL_MASS_DUMP
        {
            APILevel lev;
            GL_DUMPED_FMT dumps[6];
        };

        /* TODO: Write code for fetching program handles
         *  and extracting them, maybe share
         *  the code between paths? */
    }
}

}
}
}
