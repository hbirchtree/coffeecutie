#include <coffee/graphics/apis/gleam/rhi/gleam_shader_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>
#include "gleam_internal_types.h"

#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

static const constexpr cstring GLES20_COMPAT_VS = {
    "\n#define in attribute\n"
    "#define out varying\n"
    "#define gl_InstanceID InstanceID\n"

    "precision highp int;\n"

    "uniform int InstanceID;\n"
};

static const constexpr cstring GLES20_COMPAT_FS = {
    "#define in varying\r\n"
    "#define OutColor gl_FragColor\n"
    "#define sampler2DArray sampler2D\n"
    "#define sampler3DArray sampler3D\n"
    "#define gl_InstanceID InstanceID\n"

    "#define texture2DArray(texUnit, texCoord) "
    "texture2DArray_Internal(texUnit, texCoord, texdata_gridSize)\n"
//    "texture2DArray_Internal(texUnit, texCoord, texUnit ## _gridSize)\n"

    "precision mediump float;\n"
    "precision highp int;\n"

    "vec4 texture(sampler2D sampler, vec2 texCoord)"
    "{"
    "   return texture2D(sampler, texCoord.xy);"
    "}\n\n"
    "vec4 texture(sampler2D sampler, vec3 texCoord)"
    "{"
    "   return texture2D(sampler, texCoord.xy);"
    "}\n\n"

    "vec4 texture2DArray_Internal(sampler2D tex, vec3 coord, float gridSize)"
    "{"
    "    float squareSize = gridSize;"
    "    int iSquareSize = int(squareSize);"
    "    float fSquareSize = 1.0 / squareSize;"

    "    float gridY = float(int(coord.z) / iSquareSize) * fSquareSize;"
    "    float gridX = (coord.z - gridY * squareSize) * fSquareSize;"

    "    vec2 baseCoord = vec2(gridX * fSquareSize, gridY * fSquareSize);"

    "    coord.xy = coord.xy"
    "            * vec2(fSquareSize, fSquareSize)"
    "            + vec2(gridX, gridY);"

    "    return texture2D(tex, coord.xy);"
    "}\n\n"

    "uniform int InstanceID;\n"
};

STATICINLINE CString StringExtractLine(CString& shader, cstring query)
{
    if(shader.find(query) != CString::npos)
    {
        auto startIdx = shader.find(query);
        if(query[0] == '\n')
            startIdx++;
        auto endIdx = shader.find("\n", startIdx + 1);
        if(endIdx != CString::npos)
        {
            endIdx++;

            CString versionDirective = {};
            versionDirective.insert(0,
                                    &shader[startIdx],
                                    endIdx - startIdx);
            shader.erase(startIdx, endIdx - 1);

            return versionDirective;
        }
    }
    return {};
}

STATICINLINE void TransformShader(Bytes const& inputShader,
                                  ShaderStage stage,
                                  Vector<CString>& shaderStorage,
                                  Vector<cstring>& shaderSrcVec)
{
    CString transformedShader;
    /* Because the original shader may not be null-terminated, we do
     *  an insertion with the given length. */
    transformedShader.insert(
                0, C_FCAST<cstring>(inputShader.data),
                C_FCAST<CString::size_type>(inputShader.size));

    /* Before adding more snippets, we need to move the
     *  #version directive */
    CString versionDirective = {};
    if((versionDirective = StringExtractLine(transformedShader,
                                             "#version ")).size()
            && !GLEAM_FEATURES.gles20)
    {
        /* OpenGL GLSL ES 1.00 does not have a #version directive,
         *  remove it */
        shaderStorage.push_back(versionDirective);
    }else
    {
        shaderStorage.push_back("#version 100\n");
    }

    /* We move the extension directives at this point */
    CString extensionDirective = {};
    while((extensionDirective =
           StringExtractLine(transformedShader,
                             "\n#extension ")).size())
        shaderStorage.push_back(extensionDirective);

    if(GLEAM_FEATURES.gles20)
    {
        /* TODO: Provide better support for sampler2DArray, creating
     *  extra uniforms for grid size and etc. This will allow us
     *  to sample a sampler2D as if it were a sampler2DArray, all
     *  with the same code. */

        /* TODO: Using the sampler2DArray code, do the same for sampler3D.
     * This will require special care when providing the user with
     *  texture size limits. We may calculate the max size of a 3D
     *  texture based on 2D texture size. Most likely, this will be
     *  2048x2048 split into 128x or 256x pieces, or maybe smaller. */

        /* We add a compatibility shim for vertex and fragment shaders.
     * This does some basic conversion, such as swapping "in" and
     *  "out" with the approriate 1.00 equivalents (attribute and
     *  varying) */


        /* Remove the output declaration from modern GLSL */
        transformedShader = CStrReplace(transformedShader,
                                        "out vec4 OutColor;", "");

        /* If a line has layout(...), remove it, GLSL 1.00
     *  does not support that */
        {
            CString::size_type it;
            while((it = transformedShader.find("layout")) != CString::npos)
            {
                auto endPos = transformedShader.find(')', it);
                if(endPos != CString::npos)
                    transformedShader.erase(it, endPos + 1 - it);
            }
        }

        if(stage == ShaderStage::Vertex)
            shaderSrcVec.push_back(GLES20_COMPAT_VS);
        else
            shaderSrcVec.push_back(GLES20_COMPAT_FS);

    }else
    {

        /* Desktop GL does not require a precision specifier */

        /* For compatibility, remove usages of texture2DArray() in
         *  favor of texture(). texture2DArray() was never put into
         *  the standard. */
        shaderSrcVec.push_back(
                    "#define texture2DArray texture\n"
                    );

        if(GLEAM_API::LevelIsOfClass(GL_CURR_API, APIClass::GLES))
            shaderSrcVec.push_back(
                        "precision highp float;\n"
                        "precision highp int;\n"
                        );
    }

    /* For supporting BaseInstance */
    if(!GLEAM_FEATURES.base_instance
            && stage == ShaderStage::Vertex)
    {
        shaderSrcVec.push_back(
                    "#define gl_BaseInstanceARB BaseInstance\n"
                    "#define gl_BaseInstance BaseInstance\n"
                    "uniform int BaseInstance;\n"
                    );
    }

    shaderStorage.push_back(transformedShader);

    auto directiveEnd = shaderStorage.size() - 1;

    for(auto i : Range<>(directiveEnd))
        shaderSrcVec.insert(shaderSrcVec.begin(),
                            shaderStorage.at(directiveEnd - 1 - i).c_str());

    shaderSrcVec.push_back(shaderStorage.back().c_str());
}

bool GLEAM_Shader::compile(ShaderStage stage, const Bytes &data)
{
    if(GLEAM_FEATURES.gles20
            && stage != ShaderStage::Vertex
            && stage != ShaderStage::Fragment)
        return false;

    Vector<cstring> shaderSrcVec = {};
    Vector<CString> shaderStorage = {};

    TransformShader(data, stage, shaderStorage, shaderSrcVec);

    if(!GLEAM_FEATURES.separable_programs)
    {
        CGL33::ShaderAlloc(1,stage,&m_handle);

		if (m_handle == 0)
		{
			cWarning("Failed to allocate shader handle..?");
			return false;
        }

        Vector<i32> shaderSrcLens = {};

        shaderSrcLens.reserve(shaderSrcVec.size() + 1);
        for(cstring fragment : shaderSrcVec)
            shaderSrcLens.push_back(C_FCAST<i32>(StrLen(fragment)));

        i32* shaderLens = shaderSrcLens.data();
        cstring* shaderSrc = shaderSrcVec.data();
        u32 numSources = C_FCAST<u32>(shaderSrcVec.size());

        cVerbose(12, "Compiling shader fragment:");
        for(u32 i : Range<u32>(numSources))
            cVerbose(12, "{0}", CString(shaderSrcVec[i],
                                  shaderSrcLens[i]));

        CGL33::ShaderSource(m_handle,numSources,
                            shaderLens,
                            shaderSrc);
        bool stat = CGL33::ShaderCompile(m_handle);

        if(GL_DEBUG_MODE && !stat)
        {
            cstring_w log = CGL33::ShaderGetLog(m_handle);
            cWarning("Shader compilation error: {0}",log);
            return false;
        }

        m_stages = stage;

        return stat;
    }
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.separable_programs)
    {
        cstring* srcs = shaderSrcVec.data();

        m_handle = 0;
        m_handle = CGL43::ProgramCreate(
                    stage,
                    C_FCAST<u32>(shaderSrcVec.size()),
                    srcs);

        i32 link_state = 0;
        CGL43::ProgramGetiv(m_handle, GL_LINK_STATUS, &link_state);

        if(GL_DEBUG_MODE && (m_handle == 0 || !link_state))
        {
            cstring_w log_c = CGL43::ProgramGetLog(m_handle);
            cWarning("Shader program compilation error: {0}",log_c);
            delete[] log_c;
            return false;
        }

        m_stages = stage;

        return m_handle!=0;
    }
#endif
    else
        return false;
}

void GLEAM_Shader::dealloc()
{
    if(!GLEAM_FEATURES.separable_programs)
    {
        if(m_handle!=0)
            CGL33::ShaderFree(1,&m_handle);
    }
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.separable_programs)
    {
        if(m_handle!=0)
            CGL43::ProgramFree(1,&m_handle);
    }
#endif
}

bool GLEAM_Pipeline::attach(const GLEAM_Shader &shader,
                            const ShaderStage &stages)
{
    C_USED(stages);

    if(shader.m_handle==0)
        return false;
    if(!GLEAM_FEATURES.separable_programs)
    {
        if(m_handle==0)
            CGL33::ProgramAlloc(1,&m_handle);
        CGL33::ShaderAttach(m_handle,shader.m_handle);
        return true;
    }
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.separable_programs)
    {
        if(m_handle==0)
            CGL43::PipelineAlloc(1,&m_handle);
        bool stat = CGL43::PipelineUseStages(
                    m_handle,shader.m_stages&stages,shader.m_handle);
        if(stat)
            m_programs.push_back({&shader,shader.m_stages&stages});
        return stat;
    }
#endif
    return false;
}

GLEAM_Shader& GLEAM_Pipeline::storeShader(GLEAM_Shader &&shader)
{
    m_ownedPrograms.push_back({});

    m_ownedPrograms.back() = shader;

    return m_ownedPrograms.back();
}

bool GLEAM_Pipeline::assemble()
{
    if(m_handle==0)
        return false;
    if(!GLEAM_FEATURES.separable_programs)
    {
        bool stat = CGL33::ProgramLink(m_handle);
        if(GL_DEBUG_MODE && !stat)
        {
            cstring_w log_ = CGL33::ProgramGetLog(m_handle);
            cDebug("Program link error: {0}", log_);
            delete[] log_;
        }
        return stat;
    }
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.separable_programs)
    {
        bool stat = CGL43::PipelineValidate(m_handle);
        if(GL_DEBUG_MODE && !stat)
        {
            ShaderStage s = CGL43::PipelineGetStages(m_handle);
            cstring_w log_ = CGL43::PipelineGetLog(m_handle);
            cDebug("Pipeline validation error: {0}",log_);
            delete[] log_;
            cDebug("Pipeline stages: {0}",(uint32)s);
        }
        return stat;
    }
#endif
    return false;
}

void GLEAM_Pipeline::bind() const
{
    if(!GLEAM_FEATURES.separable_programs)
        CGL33::ProgramUse(m_handle);
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.separable_programs)
        CGL43::PipelineBind(m_handle);
#endif
}

void GLEAM_Pipeline::unbind() const
{
    if(!GLEAM_FEATURES.separable_programs)
        CGL33::ProgramUse(0);
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.separable_programs)
        CGL43::PipelineBind(0);
#endif
}

void GLEAM_Pipeline::dealloc()
{
    if(!GLEAM_FEATURES.separable_programs)
        CGL33::ProgramFree(1, &m_handle);
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.separable_programs)
        CGL43::PipelineFree(0, &m_handle);
#endif
}

bool GLEAM_ShaderUniformState::setUniform(
        const GLEAM_UniformDescriptor &value,
        GLEAM_UniformValue *data)
{
    using namespace ShaderTypes;

    if(value.m_idx<0)
        return false;

    if(value.m_flags & ShaderTypes::Uniform_v)
        (void)0x0;
    else
        return false;

    uint32 idx = value.m_idx;
    data->flags = value.m_flags;
    m_uniforms[idx] = {data,value.stages};
    return true;
}

STATICINLINE bool translate_sampler_type(
        Texture& samplerType, u32 m_flags)
{
    using namespace ShaderTypes;

    switch(m_flags & SizeMask_f)
    {
    case S2:
        samplerType = Texture::T2D;
        return true;
    case SCube:
        samplerType = Texture::Cubemap;
        return true;
    default:
        if(!GLEAM_FEATURES.gles20)
            switch(m_flags & SizeMask_f)
            {
#if !defined(COFFEE_ONLY_GLES20)
            case S3:
                samplerType = Texture::T3D;
                return true;
            case S2A:
                samplerType = Texture::T2DArray;
                return true;
            case SCubeA:
                samplerType = Texture::CubemapArray;
                return true;
#endif
            }
        if((m_flags & Depth) == Depth)
            return true;
        else
            return false;
    }
}

bool GLEAM_ShaderUniformState::setSampler(
        const GLEAM_UniformDescriptor &value,
        const GLEAM_SamplerHandle *sampler)
{
    using namespace ShaderTypes;

    if(value.m_idx<0)
        return false;

    if((value.m_flags & ShaderTypes::Sampler_v) != 0)
        (void)0x0;
    else
        return false;

    Texture samplerType = Texture::T2D;

    if(!translate_sampler_type(samplerType, value.m_flags))
        return false;

    uint32 idx = value.m_idx;
    m_samplers[idx] = {sampler,value.stages};
    return true;
}

bool GLEAM_ShaderUniformState::setUBuffer(
        const GLEAM_UniformDescriptor &value,
        const GLEAM_UniformBuffer *buf,
        GLEAM_BufferSection const& sec)
{
    using namespace ShaderTypes;

    if(value.m_idx<0)
        return false;

    if(value.m_flags & ShaderTypes::UniBuf_t)
        (void)0x0;
    else
        return false;

    uint32 idx = value.m_idx;
    m_ubuffers[idx] = {sec,buf,value.stages};
    return true;
}

bool GLEAM_ShaderUniformState::setSBuffer(
        const GLEAM_UniformDescriptor &value,
        const GLEAM_ShaderBuffer *buf,
        GLEAM_BufferSection const& sec)
{
    using namespace ShaderTypes;

    if(value.m_idx<0)
        return false;

    if(value.m_flags & ShaderTypes::ShSBuf_t)
        (void)0x0;
    else
        return false;

    uint32 idx = value.m_idx;
    m_sbuffers[idx] = {sec,buf,value.stages};
    return true;
}

void GLEAM_ShaderUniformState::clear()
{
    m_samplers.clear();
    m_sbuffers.clear();
    m_ubuffers.clear();
    m_uniforms.clear();
}

#if !defined(COFFEE_ONLY_GLES20)
STATICINLINE void ProgramInputGet(CGhnd hnd, ShaderStage stages,
                                  CGenum type,
                                  Vector<GLEAM_ProgramParameter>* params)
{
    i32 num_attrs = 0;
    if(params)
        glGetProgramInterfaceiv(hnd, type,
                                GL_ACTIVE_RESOURCES, &num_attrs);

    for(auto i : Range<>(C_FCAST<u32>(num_attrs)))
    {
        const CGenum props_to_get[] = {
            GL_NAME_LENGTH,
            GL_LOCATION,
//            GL_LOCATION_COMPONENT,
            GL_TYPE,
            GL_ARRAY_SIZE,
        };
        i32 props_out[sizeof(props_to_get)/sizeof(CGenum)];

        glGetProgramResourceiv(
                    hnd, type, C_FCAST<u32>(i),
                    sizeof(props_to_get) / sizeof(CGenum),
                    props_to_get,
                    sizeof(props_out), nullptr,
                    props_out);

        if(props_out[1] == -1)
            continue;

        params->push_back({});

        GLEAM_ProgramParameter& desc = params->back();
        desc.stages = stages;

        desc.m_idx = C_FCAST<u16>(props_out[1]);
        desc.m_flags = to_enum_shtype(C_FCAST<CGenum>(props_out[2]));
        desc.m_name.resize(C_FCAST<u32>(props_out[0]));
        glGetProgramResourceName(
                    hnd, type, C_FCAST<u32>(i),
                    props_out[0],
                nullptr, &desc.m_name[0]);
        desc.m_name.resize(desc.m_name.find('\0'));
    }
}
#endif

void GetShaderUniforms(const GLEAM_Pipeline &pipeline,
                       Vector<GLEAM_UniformDescriptor> *uniforms,
                       Vector<GLEAM_ProgramParameter> *params,
                       Vector<GLEAM_ProgramParameter> *outputs)
{
    using namespace ShaderTypes;

    if(!GLEAM_FEATURES.separable_programs)
    {
        /* Does not differentiate between shader stages and
         *  their uniforms */
        /* GL 4.3+ do not work with this */
        CGhnd prog = pipeline.m_handle;

        /* Get typical uniforms */
        if(uniforms)
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

                /* Some GLSL compilers (*cough* NVIDIA *cough*) add '[0]'
                 *  to array uniforms. */
                if(desc.m_name.find('[') != CString::npos)
                    desc.m_name.resize(desc.m_name.find('['));

                desc.m_idx = CGL33::ProgramUnifGetLoc(
                            prog,desc.m_name.c_str());
                desc.stages = ShaderStage::All;
                desc.m_flags = to_enum_shtype(v.type);
                desc.m_arrSize = v.size;
                delete[] v.name;
            }
            delete[] unifs;
        }

        /* Get uniforms buffers */
        {
        }

        /* Get attribute locations, names and types */
        if(params)
        {
            u32 num = 0;
            cstring_w* names = nullptr;
            u32* types = nullptr;
            i32* sizes = nullptr;
            CGL33::ProgramAttribGet(prog, &num, &names, &types, &sizes);
            params->reserve(num);

            for(u32 i=0;i<num;i++)
            {
                params->push_back({});
                auto& att = params->back();

                att.m_name = names[i];
                att.m_idx = C_CAST<u16>(
                            CGL33::ProgramAttribLoc(prog, names[i]));
                att.m_flags = to_enum_shtype(types[i]);
                delete[] names[i];
            }
            delete[] names;
            delete[] types;
            delete[] sizes;
        }
    }
#if !defined(COFFEE_ONLY_GLES20)
    else if(GLEAM_FEATURES.separable_programs){

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
            glGetProgramInterfaceiv(hnd, GL_UNIFORM,
                                    GL_ACTIVE_RESOURCES, &num_unifs);

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
                                       props_to_get,
                                       sizeof(props_out), nullptr,
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

                /* Some GLSL compilers (*cough* NVIDIA *cough*) add '[0]'
                 *  to array uniforms. */
                if(desc.m_name.find('[') != CString::npos)
                    desc.m_name.resize(desc.m_name.find('['));
            }

            if(params)
                ProgramInputGet(hnd, p.stages,
                                GL_PROGRAM_INPUT,
                                params);

            if(outputs)
                ProgramInputGet(hnd, p.stages,
                                GL_PROGRAM_OUTPUT,
                                outputs);
        }
    }
#endif
}

GLEAM_PipelineDumper::GLEAM_PipelineDumper(GLEAM_Pipeline &pipeline):
    PipelineDumper(pipeline)
{
}

void GLEAM_PipelineDumper::dump(cstring out)
{
    C_USED(out);

    if(GLEAM_FEATURES.gles20)
        return;

#if !defined(COFFEE_ONLY_GLES20)
    int32* bin_fmts = &GLEAM_API_INSTANCE_DATA->GL_CACHED
            .NUM_PROGRAM_BINARY_FORMATS;
    if(*bin_fmts == -1)
    {
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
            (!GLEAM_FEATURES.separable_programs))
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
        if(CResources::FileCommit(output, false, ResourceAccess::Discard))
            cVerbose(5,"Dumped program ({0}) into file {1}",
                     m_pipeline.m_handle,
                     output.resource());
        else
            cVerbose(5,"Dumping program ({0}) into file {1} failed",
                     m_pipeline.m_handle,
                     output.resource());
    }else if(GLEAM_FEATURES.separable_programs)
    {
        /* With pipeline objects we must fetch several
         *  shader programs and dump these */
        /* We also need some struct to tell us offsets of
         *  these programs and what shaders they are */
        struct GL_DUMPED_FMT
        {
            u64 offset;
            ShaderStage stg;
            u32 _pad;
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
#endif
}

}
}
}
