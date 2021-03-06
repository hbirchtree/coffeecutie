#include <coffee/graphics/apis/gleam/rhi/gleam_shader_rhi.h>

#include "gleam_internal_types.h"
#include <coffee/core/stl_types.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <peripherals/stl/string_ops.h>

namespace Coffee {
namespace RHI {
namespace GLEAM {

static const constexpr cstring GLES20_COMPAT_VS = {
    R"(

#define in attribute
#define out varying
#define flat
#define gl_InstanceID InstanceID

uniform int InstanceID;
precision highp int;

    )"};

static const constexpr cstring GLES20_COMPAT_FS = {
    R"(
#define in varying
#define flat
#define OutColor gl_FragColor
#define sampler2DArray sampler2D
#define sampler3DArray sampler3D
#define gl_InstanceID InstanceID

//#define texture2DArray(texUnit, texCoord)
//texture2DArray_Internal(texUnit, texCoord, texdata_gridSize)
//texture2DArray_Internal(texUnit, texCoord, texUnit ## _gridSize)

precision mediump float;
precision highp int;

vec4 texture(sampler2D sampler, vec2 texCoord)
{
   return texture2D(sampler, texCoord.xy);
}

vec4 texture(sampler2D sampler, vec3 texCoord)
{
   return texture2D(sampler, texCoord.xy);
}

vec4 texture2DArray_Internal(sampler2D tex, vec3 coord, float gridSize)
{
    int iSquare = int(gridSize);

    int grid_times = int(coord.z) / iSquare;
    int remainder = int(coord.z) - grid_times * iSquare;

    float gridX = float(remainder);
    float gridY = float(grid_times);

    vec2 scaledCoord = coord.xy / gridSize;

    scaledCoord = scaledCoord + vec2(gridX, gridY) / gridSize;

    scaledCoord = (scaledCoord - vec2(1.0)) / 2.0;

    return texture2D(tex, scaledCoord);
}

vec4 texture2DArray(sampler2D tex, vec3 coord)
{
    return texture2D(tex, coord.xy);
}

#define texture2D texture

)"

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
            versionDirective.insert(0, &shader[startIdx], endIdx - startIdx);
            shader.erase(startIdx, endIdx - 1);

            return versionDirective;
        }
    }
    return {};
}

STATICINLINE void TransformShader(
    Bytes const&                   inputShader,
    ShaderStage                    stage,
    Vector<CString>&               shaderStorage,
    Vector<cstring>&               shaderSrcVec,
    GLEAM_Shader::Constants const& constants)
{
    CString transformedShader;
    /* Because the original shader may not be null-terminated, we do
     *  an insertion with the given length. */
    transformedShader.insert(
        0,
        C_FCAST<cstring>(inputShader.data),
        C_FCAST<CString::size_type>(inputShader.size));

    /* Before adding more snippets, we need to move the
     *  #version directive */
    CString versionDirective = {};
    if((versionDirective = StringExtractLine(transformedShader, "#version "))
           .size() &&
       !GLEAM_FEATURES.gles20)
    {
        /* OpenGL GLSL ES 1.00 does not have a #version directive,
         *  remove it */
        shaderStorage.push_back(versionDirective);
    } else
    {
        shaderStorage.push_back("#version 100\n");
    }

    if(GLEAM_OPTIONS.old_shader_processing)
    {
        /* We move the extension directives at this point */
        CString extensionDirective = {};
        while((extensionDirective =
                   StringExtractLine(transformedShader, "\n#extension "))
                  .size())
            shaderStorage.push_back(extensionDirective);
    }

    if(GLEAM_FEATURES.gles20 && GLEAM_OPTIONS.old_shader_processing)
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
        transformedShader =
            str::replace::str(transformedShader, "out vec4 OutColor;", "");

        transformedShader =
            str::replace::str(transformedShader, "uniform int InstanceID;", "");

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

    } else if(GLEAM_OPTIONS.old_shader_processing)
    {
        /* Desktop GL does not require a precision specifier */

        /* For compatibility, remove usages of texture2DArray() in
         *  favor of texture(). texture2DArray() was never put into
         *  the standard. */
        shaderSrcVec.push_back("#define texture2DArray texture\n");

        if(GLEAM_API::LevelIsOfClass(GL_CURR_API, APIClass::GLES))
            shaderSrcVec.push_back("precision highp float;\n"
                                   "precision highp int;\n");
    }

    /* For supporting BaseInstance */
    if(!GLEAM_FEATURES.base_instance && stage == ShaderStage::Vertex &&
       GLEAM_OPTIONS.old_shader_processing)
    {
        shaderSrcVec.push_back("#define gl_BaseInstanceARB BaseInstance\n"
                               "#define gl_BaseInstance BaseInstance\n"
                               "uniform int BaseInstance;\n");
    }
    if(stage == ShaderStage::Vertex && !GLEAM_OPTIONS.old_shader_processing)
    {
        shaderSrcVec.push_back(
            R"(
#define SPIRV_Cross_BaseInstance BaseInstance
#define SPIRV_Cross_InstanceID InstanceID
)");
    }

    for(auto const& constant : constants)
        shaderStorage.push_back(
            "#define " + constant.first + " " +
            str::convert::to_string(constant.second));

    shaderStorage.push_back(transformedShader);

    auto directiveEnd = shaderStorage.size() - 1;

    for(auto i : Range<>(directiveEnd))
        shaderSrcVec.insert(
            shaderSrcVec.begin(),
            shaderStorage.at(directiveEnd - 1 - i).c_str());

    shaderSrcVec.push_back(shaderStorage.back().c_str());
    shaderSrcVec.push_back("\n");
}

STATICINLINE bool translate_sampler_type(tex::flag& samplerType, u32 m_flags)
{
    using namespace ShaderTypes;

    switch(m_flags & SizeMask_f)
    {
    case S2:
        samplerType = tex::t2d::value;
        return true;
    case SCube:
        samplerType = tex::cube::value;
        return true;
    default:
        if(!GLEAM_FEATURES.gles20)
            switch(m_flags & SizeMask_f)
            {
#if GL_VERSION_VERIFY(0x330, 0x300)
            case S3:
                samplerType = tex::t3d::value;
                return true;
            case S2A:
                samplerType = tex::t2d_array::value;
                return true;
#endif
#if GL_VERSION_VERIFY(0x330, 0x320)
            case SCubeA:
                samplerType = tex::cube_array::value;
                return true;
#endif
            }
        if((m_flags & Depth) == Depth)
            return true;
        else
            return false;
    }
}

#if GL_VERSION_VERIFY(0x330, 0x310) && !defined(COFFEE_WEBGL)
STATICINLINE void ProgramInputGet(
    glhnd const&                    hnd,
    ShaderStage                     stages,
    CGenum                          type,
    Vector<GLEAM_ProgramParameter>* params)
{
    i32 num_attrs = 0;
    if(params)
        CGL43::ProgramGetInterfaceiv(
            hnd, type, GL_ACTIVE_RESOURCES, &num_attrs);

    for(auto i : Range<>(C_FCAST<u32>(num_attrs)))
    {
        const CGenum props_to_get[] = {
            GL_NAME_LENGTH,
            GL_LOCATION,
            //            GL_LOCATION_COMPONENT,
            GL_TYPE,
            GL_ARRAY_SIZE,
        };
        i32 props_out[sizeof(props_to_get) / sizeof(CGenum)];

        CGL43::ProgramGetResourceiv(
            hnd,
            type,
            C_FCAST<u32>(i),
            sizeof(props_to_get) / sizeof(CGenum),
            props_to_get,
            sizeof(props_out),
            nullptr,
            props_out);

        if(props_out[1] == -1)
            continue;

        params->push_back({});

        GLEAM_ProgramParameter& desc = params->back();
        desc.stages                  = stages;

        desc.m_idx   = C_FCAST<u16>(props_out[1]);
        desc.m_flags = to_enum_shtype(C_FCAST<CGenum>(props_out[2]));
        desc.m_name.resize(C_FCAST<u32>(props_out[0]));
        CGL43::ProgramGetResourceName(
            hnd, type, C_FCAST<u32>(i), props_out[0], nullptr, &desc.m_name[0]);
        desc.m_name.resize(desc.m_name.find('\0'));
    }
}
#endif

bool GLEAM_Shader::compile(
    ShaderStage      stage,
    const Bytes&     data,
    gleam_error&     ec,
    Constants const& constants)
{
    if(data.size == 0)
    {
        ec = APIError::ShaderNoData;
        return false;
    }

    if(GLEAM_FEATURES.gles20 && stage != ShaderStage::Vertex &&
       stage != ShaderStage::Fragment)
    {
        ec = APIError::ShaderStageNotValid;
        return false;
    }

    Vector<cstring> shaderSrcVec  = {};
    Vector<CString> shaderStorage = {};

    TransformShader(data, stage, shaderStorage, shaderSrcVec, constants);

    if(!GLEAM_FEATURES.separable_programs)
    {
        m_handle = CGL33::ShaderAllocEx(stage);

        if(!m_handle)
        {
            ec = APIError::HandleAllocationFailed;
            return false;
        }

        Vector<i32> shaderSrcLens = {};

        shaderSrcLens.reserve(shaderSrcVec.size() + 1);
        for(cstring fragment : shaderSrcVec)
            shaderSrcLens.push_back(C_FCAST<i32>(libc::str::len(fragment)));

        const i32*     shaderLens = shaderSrcLens.data();
        cstring* const shaderSrc  = shaderSrcVec.data();
        u32            numSources = C_FCAST<u32>(shaderSrcVec.size());

        CGL33::ShaderSource(m_handle, numSources, shaderSrc, shaderLens);

        CGL33::ShaderCompile(m_handle);

        i32 stat = 0;
        CGL33::ShaderGetiv(m_handle, GL_COMPILE_STATUS, &stat);

        if(stat != GL_TRUE)
        {
            if(GL_DEBUG_MODE)
            {
                CString infoLog;
                i32     logLen = 0;
                i32     dummy  = 0;

                CGL33::ShaderGetiv(m_handle, GL_INFO_LOG_LENGTH, &logLen);
                logLen++;
                infoLog.resize(C_FCAST<szptr>(logLen));
                CGL33::ShaderGetInfoLog(m_handle, logLen, &dummy, &infoLog[0]);
                szptr idx = 0;
                if((idx = infoLog.find('\0')) != CString::npos)
                {
                    infoLog.resize(infoLog.find('\0'));
                    ec = infoLog;
                }
            }

            dealloc(ec);
            ec = APIError::ShaderCompileFailed;
            return false;
        }

        m_stages = stage;

        return true;
    }
#if GL_VERSION_VERIFY(0x330, 0x320)
    else if(GLEAM_FEATURES.separable_programs)
    {
        m_handle = CGL43::ShaderProgramvAllocEx(stage, shaderSrcVec);

        i32 link_state = 0;
        CGL43::ProgramGetiv(m_handle, GL_VALIDATE_STATUS, &link_state);

        if(!m_handle || link_state != GL_TRUE)
        {
            if(GL_DEBUG_MODE)
            {
                CString infoLog;
                i32     logLen = 0;

                CGL43::ProgramGetiv(m_handle, GL_INFO_LOG_LENGTH, &logLen);
                logLen++;
                infoLog.resize(C_FCAST<szptr>(logLen));
                CGL43::ProgramGetInfoLog(
                    m_handle, logLen, nullptr, &infoLog[0]);

                szptr idx;

                if((idx = infoLog.find('\0')) != CString::npos)
                {
                    infoLog.resize(infoLog.find('\0'));
                    ec = infoLog;
                }
            }

            dealloc(ec);
            ec = APIError::ShaderCompileFailed;
            return false;
        }

        m_stages = stage;

        return true;
    }
#endif
    else
    {
        return false;
    }
}

void GLEAM_Shader::dealloc(gleam_error& ec)
{
    if(!GLEAM_FEATURES.separable_programs)
    {
        if(!m_handle)
            ec = APIError::HandleDeallocFailed;
        else
            CGL33::ShaderFree(m_handle);
    }
#if GL_VERSION_VERIFY(0x410, 0x300)
    else if(GLEAM_FEATURES.separable_programs)
    {
        if(!m_handle)
            ec = APIError::HandleDeallocFailed;
        else
            CGL43::ProgramFree(m_handle);
    }
#endif
    m_handle.release();
}

bool GLEAM_Pipeline::attach(
    const GLEAM_Shader& shader,
    C_UNUSED(const ShaderStage& stages),
    gleam_error& ec)
{
    if(!shader.m_handle)
    {
        ec = APIE::InvalidObject;
        return false;
    }

    if(!GLEAM_FEATURES.separable_programs)
    {
        if(!m_handle)
            m_handle = CGL33::ProgramAllocEx();
        CGL33::ShaderAttach(m_handle, shader.m_handle);
        return true;
    }
#if GL_VERSION_VERIFY(0x330, 0x320)
    else if(GLEAM_FEATURES.separable_programs)
    {
        if(!m_handle)
        {
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
            if(GLEAM_FEATURES.direct_state)
                CGL45::PipelineAllocEx(m_handle.hnd);
            else
#endif
                CGL43::PipelineAlloc(m_handle.hnd);
        }
        CGL43::ProgramUseStages(
            m_handle, shader.m_stages & stages, shader.m_handle);
        m_programs.push_back({&shader, shader.m_stages & stages});

        return true;
    }
#endif
    return false;
}

GLEAM_Shader& GLEAM_Pipeline::storeShader(GLEAM_Shader&& shader)
{
    m_ownedPrograms.emplace_back();

    m_ownedPrograms.back() = std::move(shader);

    return m_ownedPrograms.back();
}

bool GLEAM_Pipeline::assemble(gleam_error& ec)
{
    if(!m_handle)
    {
        ec = APIError::InvalidObject;
        return false;
    }

    if(!GLEAM_FEATURES.separable_programs)
    {
        CGL33::ProgramLink(m_handle);
        i32 stat = 0;
        CGL33::ProgramGetiv(m_handle, GL_LINK_STATUS, &stat);
        if(GL_DEBUG_MODE && !stat)
        {
            CString infoLog;
            i32     logLen;
            CGL33::ProgramGetiv(m_handle, GL_INFO_LOG_LENGTH, &logLen);
            infoLog.resize(C_FCAST<szptr>(logLen));
            CGL33::ProgramGetInfoLog(m_handle, logLen, nullptr, &infoLog[0]);
            ec = APIError::PipelineLinkError;
            ec = infoLog;
        }
        return stat;
    }
#if GL_VERSION_VERIFY(0x330, 0x320)
    else if(GLEAM_FEATURES.separable_programs)
    {
        i32 stat = 0;
        CGL43::PipelineValidate(m_handle);
        CGL43::PipelineGetiv(m_handle, GL_VALIDATE_STATUS, &stat);
        if(GL_DEBUG_MODE && !stat)
        {
            CString infoLog;
            i32     logLen;
            CGL43::PipelineGetiv(m_handle, GL_INFO_LOG_LENGTH, &logLen);
            infoLog.resize(C_FCAST<szptr>(logLen));
            CGL43::PipelineGetInfoLog(m_handle, logLen, nullptr, &infoLog[0]);
            ec = APIError::PipelineValidationError;
            ec = infoLog;
        }
        return stat;
    }
#endif
    ec = APIE::UnimplementedPath;
    return false;
}

void GLEAM_Pipeline::bind(gleam_error& ec) const
{
    if(!GLEAM_FEATURES.separable_programs)
    {
        CGL33::ProgramUse(m_handle);
        return;
    }
#if GL_VERSION_VERIFY(0x330, 0x320)
    else if(GLEAM_FEATURES.separable_programs)
    {
        CGL43::PipelineBind(m_handle);
        return;
    }
#endif
    ec = APIE::UnimplementedPath;
}

void GLEAM_Pipeline::unbind(gleam_error& ec) const
{
    if(!GLEAM_FEATURES.separable_programs)
    {
        CGL33::ProgramUse(glhnd());
        return;
    }
#if GL_VERSION_VERIFY(0x330, 0x320)
    else if(GLEAM_FEATURES.separable_programs)
    {
        CGL43::PipelineBind(glhnd());
        return;
    }
#endif
    ec = APIE::UnimplementedPath;
}

void GLEAM_Pipeline::dealloc(gleam_error& ec)
{
    for(auto& shader : m_ownedPrograms)
        shader.dealloc(ec);

    if(!GLEAM_FEATURES.separable_programs)
        CGL33::ProgramFree(m_handle);
#if GL_VERSION_VERIFY(0x330, 0x320)
    else if(GLEAM_FEATURES.separable_programs)
        CGL43::PipelineFree(m_handle.hnd);
#endif
    m_handle.release();
}

bool GLEAM_ShaderUniformState::setUniform(
    const GLEAM_UniformDescriptor& value, GLEAM_UniformValue* data)
{
    using namespace ShaderTypes;

    if(value.m_idx < 0)
        return false;

    if(value.m_flags & ShaderTypes::Uniform_v)
        (void)0x0;
    else
        return false;

    u32 idx         = value.m_idx;
    data->flags     = value.m_flags;
    m_uniforms[idx] = {data, value.stages};
    return true;
}

bool GLEAM_ShaderUniformState::setSampler(
    const GLEAM_UniformDescriptor& value, const GLEAM_SamplerHandle* sampler)
{
    using namespace ShaderTypes;

    if(value.m_idx < 0)
        return false;

    if((value.m_flags & ShaderTypes::Sampler_v) != 0)
        (void)0x0;
    else
        return false;

    tex::flag samplerType = tex::t2d::value;

    if(!translate_sampler_type(samplerType, value.m_flags))
        return false;

    u32 idx         = value.m_idx;
    m_samplers[idx] = {sampler, value.stages};
    return true;
}

bool GLEAM_ShaderUniformState::setUBuffer(
    const GLEAM_UniformDescriptor& value,
    const GLEAM_UniformBuffer*     buf,
    GLEAM_BufferSection const&     sec)
{
    using namespace ShaderTypes;

    if(value.m_idx < 0)
        return false;

    if(value.m_flags & ShaderTypes::UniBuf_t)
        (void)0x0;
    else
        return false;

    u32 idx         = value.m_idx;
    m_ubuffers[idx] = {sec, buf, value.stages};
    return true;
}

bool GLEAM_ShaderUniformState::setSBuffer(
    const GLEAM_UniformDescriptor& value,
    const GLEAM_ShaderBuffer*      buf,
    GLEAM_BufferSection const&     sec)
{
    using namespace ShaderTypes;

    if(value.m_idx < 0)
        return false;

    if(value.m_flags & ShaderTypes::ShSBuf_t)
        (void)0x0;
    else
        return false;

    u32 idx         = value.m_idx;
    m_sbuffers[idx] = {sec, buf, value.stages};
    return true;
}

void GLEAM_ShaderUniformState::clear()
{
    m_samplers.clear();
    m_sbuffers.clear();
    m_ubuffers.clear();
    m_uniforms.clear();
}

void GetShaderUniforms(
    const GLEAM_Pipeline&            pipeline,
    Vector<GLEAM_UniformDescriptor>* uniforms,
    Vector<GLEAM_ProgramParameter>*  params,
    Vector<GLEAM_ProgramParameter>*  outputs)
{
    GLEAM_API::DBG::SCOPE _(GLM_API "GetShaderUniforms");

    using namespace ShaderTypes;

    if(!GLEAM_FEATURES.separable_programs || GLEAM_FEATURES.webgl)
    {
        /* Does not differentiate between shader stages and
         *  their uniforms */
        /* GL 4.3+ do not work with this */
        glhnd const& prog = pipeline.m_handle;

        /* Get typical uniforms */
        if(uniforms)
        {
            i32 num_uniforms = 0;
            i32 max_namelen  = 0;

            CGL33::ProgramGetiv(prog, GL_ACTIVE_UNIFORMS, &num_uniforms);
            CGL33::ProgramGetiv(
                prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_namelen);

            uniforms->resize(num_uniforms);

            szptr max_namelen_sz = C_FCAST<szptr>(max_namelen);

            for(auto i : Range<>(C_FCAST<szptr>(num_uniforms)))
            {
                auto& unif = uniforms->at(i);
                unif       = {};

                CString& unifName = unif.m_name;
                unifName.resize(max_namelen_sz);

                u32  type = GL_NONE;
                i32& size = unif.m_arrSize;

                CGL33::ActiveUnifGet(
                    prog,
                    i,
                    unifName.size(),
                    nullptr,
                    &size,
                    &type,
                    &unifName[0]);

                unif.m_idx   = CGL33::UnifGetLocation(prog, unifName.data());
                unif.m_flags = to_enum_shtype(type);
                unif.stages  = ShaderStage::All;

                /* If array specifier is in name, remove it */
                auto trimIdx = unifName.find('[');

                if(trimIdx != CString::npos)
                    unifName.resize(trimIdx);
                else
                    unifName.resize(unifName.find('\0'));
            }
        }

#if GL_VERSION_VERIFY(0x310, 0x300)
        /* Get uniforms buffers */
        if(uniforms)
        {
            i32 num_blocks  = 0;
            i32 max_namelen = 0;

            CGL33::ProgramGetiv(prog, GL_ACTIVE_UNIFORM_BLOCKS, &num_blocks);
            CGL33::ProgramGetiv(
                prog, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_namelen);

            szptr max_namelen_sz = C_FCAST<szptr>(max_namelen);
            szptr ublk_idx       = uniforms->size();
            uniforms->resize(uniforms->size() + C_FCAST<szptr>(num_blocks));

            for(auto i : Range<>(C_FCAST<szptr>(num_blocks)))
            {
                auto& unif    = uniforms->at(ublk_idx + i);
                auto& blkName = unif.m_name;

                blkName.resize(max_namelen_sz);

                i32 blkSize = 0, activeUnifs = 0;

                Vector<i32> unifIndices;

                CGL33::ActiveUnifBlockGetName(
                    prog, i, blkName.size(), nullptr, &blkName[0]);
                CGL33::ActiveUnifBlockGetiv(
                    prog, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blkSize);
                CGL33::ActiveUnifBlockGetiv(
                    prog, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUnifs);

                unifIndices.resize(activeUnifs);
                CGL33::ActiveUnifBlockGetiv(
                    prog,
                    i,
                    GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
                    &unifIndices.at(0));

//                Throw(implementation_error(
//                    GLM_API "Uniform block handling not implemented"));
            }
        }
#endif

        /* Get attribute locations, names and types */
        if(params)
        {
            i32 num_attributes = 0;
            i32 max_namelen    = 0;

            CGL33::ProgramGetiv(prog, GL_ACTIVE_ATTRIBUTES, &num_attributes);
            CGL33::ProgramGetiv(
                prog, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_namelen);

            szptr max_namelen_sz = C_FCAST<szptr>(max_namelen);
            params->resize(C_FCAST<szptr>(num_attributes));

            for(auto i : Range<>(C_FCAST<szptr>(num_attributes)))
            {
                auto& attrib = params->at(i);

                auto& attribName = attrib.m_name;
                attribName.resize(max_namelen_sz);

                i32 esize = 0;
                u32 type  = GL_NONE;

                CGL33::ActiveAttribGet(
                    prog,
                    i,
                    attribName.size(),
                    nullptr,
                    &esize,
                    &type,
                    &attribName[0]);

                auto idx = attribName.find('\0');
                if(idx != CString::npos)
                    attribName.resize(idx);

                attrib.m_idx =
                    CGL33::AttribGetLocation(prog, attribName.c_str());
                attrib.m_flags = to_enum_shtype(type);
                attrib.stages  = ShaderStage::All;
            }
        }
    }
#if GL_VERSION_VERIFY(0x430, 0x310) && !defined(COFFEE_WEBGL)
    else if(GLEAM_FEATURES.separable_programs)
    {
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
            int32        num_unifs;
            glhnd const& hnd = p.shader->m_handle;

            /* Get number of uniform variables */
            CGL43::ProgramGetInterfaceiv(
                hnd, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_unifs);

            for(int32 i = 0; i < num_unifs; i++)
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
                int32 props_out[sizeof(props_to_get) / sizeof(CGenum)];
                CGL43::ProgramGetResourceiv(
                    hnd,
                    GL_UNIFORM,
                    i,
                    sizeof(props_to_get) / sizeof(CGenum),
                    props_to_get,
                    sizeof(props_out),
                    nullptr,
                    props_out);

                /* Create the uniform value */
                uniforms->push_back({});
                GLEAM_UniformDescriptor& desc = uniforms->back();
                desc.stages                   = p.stages;

                desc.m_flags = to_enum_shtype(props_out[G_Type]);
                desc.m_idx   = props_out[G_Loc];

                if((desc.m_blkIdx = props_out[G_BlkIdx]) != -1)
                {
                    desc.m_blkIdx = props_out[G_BlkIdx];
                    desc.m_offset = props_out[G_Offset];
                }
                if(props_out[G_ArrSize] != -1)
                {
                    desc.m_arrSize   = props_out[G_ArrSize];
                    desc.m_arrStride = props_out[G_ArrStride];
                }

                /* Get the uniform name */
                desc.m_name.resize(props_out[G_NameLen]);
                CGL43::ProgramGetResourceName(
                    hnd,
                    GL_UNIFORM,
                    i,
                    props_out[G_NameLen],
                    nullptr,
                    &desc.m_name[0]);
                /* Remove the null character */
                if(desc.m_name.size() > 1)
                    desc.m_name.resize(desc.m_name.size() - 1);

                /* Some GLSL compilers (*cough* NVIDIA *cough*) add '[0]'
                 *  to array uniforms. */
                if(desc.m_name.find('[') != CString::npos)
                    desc.m_name.resize(desc.m_name.find('['));
            }

            if(params)
                ProgramInputGet(hnd, p.stages, GL_PROGRAM_INPUT, params);

            if(outputs)
                ProgramInputGet(hnd, p.stages, GL_PROGRAM_OUTPUT, outputs);
        }
    }
#endif
}

GLEAM_PipelineDumper::GLEAM_PipelineDumper(GLEAM_Pipeline& pipeline) :
    PipelineDumper(pipeline)
{
}

void GLEAM_PipelineDumper::dump(C_UNUSED(cstring out))
{
    // TODO: This function

    if(GLEAM_FEATURES.gles20)
        return;

#if GL_VERSION_VERIFY(0x330, 0x300)
    int32* bin_fmts =
        &GLEAM_API_INSTANCE_DATA->GL_CACHED.NUM_PROGRAM_BINARY_FORMATS;
    if(*bin_fmts == -1)
    {
        CGL::Debug::GetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, bin_fmts);
    }
    if(*bin_fmts <= 0)
    {
        //        cVerbose(6, "No GL program binary formats supported");
        return;
    }
    if(Extensions::GetProgramBinarySupported(CGL_DBG_CTXT) &&
       (!GLEAM_FEATURES.separable_programs))
    {
        CGenum         t = GL_NONE;
        Vector<byte_t> program_data;
        i32            progLen = 0;

        /* Just dump the program binary, nothing else is needed */
        Resource output(out, RSCA::NewFile);

        /* We'll fit the binary type in here */
        program_data.resize(sizeof(GL_CURR_API) + sizeof(CGenum));
        //        cVerbose(6, "About to get program binary");
        /* Append program data */
        CGL43::ProgramGetiv(
            m_pipeline.m_handle, GL_PROGRAM_BINARY_LENGTH, &progLen);

        program_data.resize(program_data.size() + C_FCAST<szptr>(progLen));

        CGL43::ProgramGetBinary(
            m_pipeline.m_handle, &progLen, &t, program_data);
        if(!progLen)
            return;

        //        cVerbose(6, "Acquired program binary");
        /* Insert API version */
        //        MemCpy(&program_data[0], &GL_CURR_API, sizeof(GL_CURR_API));
        /* Put binary type in there */
        //        MemCpy(&program_data[sizeof(GL_CURR_API)], &t, sizeof(t));

        /* Create the output resource */
        output.size = program_data.size();
        output.data = program_data.data();
        if(!FileCommit(output, RSCA::Discard))
            return;
        //            cVerbose(
        //                5,
        //                "Dumped program ({0}) into file {1}",
        //                m_pipeline.m_handle.hnd,
        //                output.resource());
        //        else
        //            cVerbose(
        //                5,
        //                "Dumping program ({0}) into file {1} failed",
        //                m_pipeline.m_handle.hnd,
        //                output.resource());
    } else if(GLEAM_FEATURES.separable_programs)
    {
        /* With pipeline objects we must fetch several
         *  shader programs and dump these */
        /* We also need some struct to tell us offsets of
         *  these programs and what shaders they are */
        struct GL_DUMPED_FMT
        {
            u64         offset;
            ShaderStage stg;
            u32         _pad;
        };
        struct GL_MASS_DUMP
        {
            APILevel      lev;
            GL_DUMPED_FMT dumps[6];
        };

        /* TODO: Write code for fetching program handles
         *  and extracting them, maybe share
         *  the code between paths? */
    }
#endif
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
