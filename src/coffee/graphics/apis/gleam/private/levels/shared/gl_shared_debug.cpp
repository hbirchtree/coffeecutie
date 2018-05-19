#include <coffee/graphics/apis/gleam/levels/shared/gl_shared_debug.h>
#include <coffee/core/base/textprocessing/cregex.h>
#include <coffee/core/plat/plat_memory.h>
#include <coffee/core/string_casting.h>

#if defined(COFFEE_USE_APPLE_GLKIT)
#include <CEAGL/eagl.h>
#elif defined(COFFEE_USE_MAEMO_EGL)
#include <EGL/egl.h>
#endif

namespace Coffee{
namespace CGL{

void CGL_Shared_Debug::SetDebugMode(bool enabled)
{
    C_USED(enabled);
#if GL_VERSION_VERIFY(0x330, 0x320)
    if(enabled == b_isDebugging)
        return;
    if(enabled)
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        b_isDebugging = true;
    }
    else
    {
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        b_isDebugging = true;
    }
#endif
}

void CGL_Shared_Debug::GetExtensions()
{
#if GL_VERSION_VERIFY(0x330, 0x300)
    int32 numExtensions = 0;
    numExtensions = GetInteger(GL_NUM_EXTENSIONS);

    if (numExtensions <= 0)
        return;

    s_ExtensionList = CString();
    s_ExtensionList.reserve(C_CAST<size_t>(numExtensions*20));
    for(int32 i=0;i<numExtensions;i++)
    {
        cstring str = GetStringi(GL_EXTENSIONS,C_CAST<uint32>(i));
        s_ExtensionList.append(str);
        if(i<numExtensions-1)
            s_ExtensionList.push_back(' ');
    }
#elif defined(COFFEE_USE_MAEMO_EGL) && !defined(COFFEE_USE_APPLE_GLKIT)
    EGLDisplay m_disp = eglGetCurrentDisplay();
    s_ExtensionList = CString();
    cstring extensions = eglQueryString(m_disp, EGL_EXTENSIONS);
    if(extensions)
        s_ExtensionList = extensions;
#endif
}

Display::CGLVersion CGL_Shared_Debug::ContextVersion()
{
    Display::CGLVersion ver = {};

    ver.major = 0;
    ver.minor = 0;
    ver.revision = 0;

    /* In most cases, this will work wonders */
#if GL_VERSION_VERIFY(0x300, 0x300)
    ver.major = C_CAST<uint8>(GetInteger(GL_MAJOR_VERSION));
    ver.minor = C_CAST<uint8>(GetInteger(GL_MINOR_VERSION));
#else
    ver.major = 2;
    ver.minor = 0;
#endif

    /* In some cases, we run on drivers that are old or crappy.
         * We still want to know what the hardware supports, though. */
    do
    {
        cstring str_ = GetString(GL_VERSION);
        if(!str_)
            break;
        CString str = str_;

        //            cVerbose(7,"Input GL_VERSION string: {0}",str);

        if (str.size()<=0)
            break;

#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
        Regex::Pattern p = Regex::Compile("([0-9]+)\\.([0-9]+)\\.([0-9])?([\\s\\S]*)");
#else
        Regex::Pattern p = Regex::Compile("OpenGL ES ([0-9]+)\\.([0-9]+)?([\\s\\S]*)");
#endif
        auto match = Regex::Match(p,str,true);
        if (match.size() < 3)
            break;
        ver = {};
        if(ver.major == 0)
            ver.major = cast_string<uint8>(match.at(1).s_match.front());
        if(ver.minor == 0)
            ver.minor = cast_string<uint8>(match.at(2).s_match.front());
        if (match.size() == 4)
        {
            bool ok = false;
            ver.revision = cast_string<uint8>(match.at(3).s_match.front());
            if (!ok)
            {
                ver.driver = StrUtil::trim(match.at(3).s_match.front());
            }
        }
        else if (match.size() == 5)
        {
            ver.revision = cast_string<uint8>(match.at(3).s_match.front());
            ver.driver = StrUtil::trim(match.at(4).s_match.front());
        }
    }while(false);

    return ver;
}

Display::CGLVersion CGL_Shared_Debug::ShaderLanguageVersion()
{
    Display::CGLVersion ver = {};

#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
    ver.revision = 1;
#endif

#if GL_VERSION_VERIFY(0x300, 0x300)
    ver.major = 1;
    ver.minor = 0;
#endif

    cstring str_c = GetString(GL_SHADING_LANGUAGE_VERSION);

    if (!str_c)
        return ver;

    //        cVerbose(7,"Input GL_SHADING_LANGUAGE_VERSION string: {0}",str_c);

    CString str = str_c;

    ver.driver = str;

    if(str.size()<1)
        return ver;

    if(ver.major == 0 && ver.minor ==0)
        do
    {
        Regex::Pattern p = Regex::Compile("([0-9]+)\\.([0-9]+).([\\s\\S]*)");
        auto match = Regex::Match(p,str,true);

        if(match.size() < 3)
            break;

        ver.major = cast_string<uint8>(match.at(1).s_match.front());
        ver.minor = cast_string<uint8>(match.at(2).s_match.front());

        if(match.size() < 4)
            break;

        ver.driver = match.at(3).s_match.front().c_str();

    }while(false);

    if(ver.major == 0 && ver.minor == 0)
        do
    {
        Regex::Pattern pa = Regex::Compile("([\\s\\S]+) ([0-9]+).([0-9]+)");
        auto match = Regex::Match(pa,str,true);

        if(match.size() == 3)
        {
            ver.major = cast_string<uint8>(match.at(1).s_match.front());
            ver.minor = cast_string<uint8>(match.at(2).s_match.front());
            break;
        }else if(match.size() ==4)
        {
            ver.major = cast_string<uint8>(match.at(2).s_match.front());
            ver.minor = cast_string<uint8>(match.at(3).s_match.front());
        }else
            break;

        ver.driver = match.at(1).s_match.front().c_str();

    }while(false);

    return ver;
}

HWDeviceInfo CGL_Shared_Debug::Renderer()
{
    cstring vendor = GetString(GL_VENDOR);
    cstring device = GetString(GL_RENDERER);
    CString driver = ContextVersion().driver;

#if defined(COFFEE_USE_MAEMO_EGL) && !defined(COFFEE_USE_APPLE_GLKIT)
    EGLDisplay m_disp = eglGetCurrentDisplay();
    if(!vendor)
        vendor = eglQueryString(m_disp, EGL_VENDOR);
#if defined(COFFEE_MAEMO)
    cstring driver_c = eglQueryString(m_disp, EGL_VERSION);
    if(!device)
        device = "PowerVR SGX 530";
    if(driver.size()<1 && driver_c)
        driver = driver_c;
#else
    if(!device)
        device = "GPU";
#endif
#endif

    if(!vendor || !device)
        return HWDeviceInfo("Unknown", "Unknown", "");

    return HWDeviceInfo(vendor,device,driver);
}

bool CGL_Shared_Debug::CompressedFormatSupport(Texture, PixelFormat t)
{
    switch(t)
    {
    case PixFmt::S3TC:
        return CheckExtensionSupported(
                    "GL_EXT_texture_compression_s3tc");
    case PixFmt::ASTC:
        return CheckExtensionSupported(
                    "GL_KHR_texture_compression_astc_hdr");
    case PixFmt::BPTC:
        return CheckExtensionSupported(
                    "GL_ARB_texture_compression_bptc");
    case PixFmt::RGTC:
        return CheckExtensionSupported(
                    "GL_ARB_texture_compression_rgtc");
    case PixFmt::ETC1:
        return CheckExtensionSupported(
                    "GL_OES_compressed_ETC1_RGB8_texture");
    default:
        break;
    }

    /* TODO: GL_COMPRESSED_TEXTURE_FORMATS */
    i32 target = C_CAST<i32>(to_enum(t));
    for(int32 i=0;i<Num_Internal_Formats;i++)
        if(target == Internal_Formats[i])
        {
            return true;
        }
    return false;
}

using L = CGL_Shared_Limits;

#define A(v1, v2) {v1, {#v2, v2}}

struct limit_name_t
{
    cstring name;
    GLuint i;
};

static Map<u32, limit_name_t> limit_map = {
    /* Baseline OpenGL ES 2.0 limits */
    A(L::Vertex_Attribs, GL_MAX_VERTEX_ATTRIBS),

    A(L::Tex_Size2D, GL_MAX_TEXTURE_SIZE),

    A(L::Vertex_Base + L::ImageUnits,GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS),
    A(L::Vertex_Base + L::UniformVectors,GL_MAX_VERTEX_UNIFORM_VECTORS),

    A(L::Fragment_Base + L::UniformVectors,GL_MAX_FRAGMENT_UNIFORM_VECTORS),

    A(L::Total_Base + L::ImageUnits, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS),

    #if GL_VERSION_VERIFY(0x330, 0x300)
    A(L::Vertex_Base + L::UniformVals,GL_MAX_VERTEX_UNIFORM_COMPONENTS),
    A(L::Vertex_Base + L::UniformsBlocks,GL_MAX_VERTEX_UNIFORM_BLOCKS),
    A(L::Vertex_Base + L::Inputs,GL_MAX_VERTEX_ATTRIBS),
    A(L::Vertex_Base + L::Outputs,GL_MAX_VERTEX_OUTPUT_COMPONENTS),
    A(L::Fragment_Base+L::UniformVals,GL_MAX_FRAGMENT_UNIFORM_COMPONENTS),
    A(L::Fragment_Base + L::UniformsBlocks,GL_MAX_FRAGMENT_UNIFORM_BLOCKS),
    A(L::Fragment_Base + L::Inputs,GL_MAX_FRAGMENT_INPUT_COMPONENTS),
    A(L::Fragment_Base + L::Outputs,GL_MAX_DRAW_BUFFERS),

    #if GL_VERSION_VERIFY(0x330, 0x320)
    A(L::Vertex_Base + L::AtomicCounters,GL_MAX_VERTEX_ATOMIC_COUNTERS),
    A(L::Vertex_Base + L::AtomicBufs,GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS),
    A(L::Vertex_Base + L::SSBO,GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS),
    A(L::Vertex_Base + L::ImageUniforms,GL_MAX_VERTEX_IMAGE_UNIFORMS),
    A(L::Fragment_Base+L::AtomicCounters,GL_MAX_FRAGMENT_ATOMIC_COUNTERS),
    A(L::Fragment_Base+L::AtomicBufs,GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS),
    A(L::Fragment_Base + L::SSBO,GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS),
    A(L::Fragment_Base + L::ImageUniforms,GL_MAX_FRAGMENT_IMAGE_UNIFORMS),
    #endif

    #endif

    #if GL_VERSION_VERIFY(0x330, 0x300)
    A(L::Vertex_Base + L::Outputs, GL_MAX_VARYING_VECTORS),
    A(L::Fragment_Base + L::Inputs, GL_MAX_VARYING_VECTORS),
    #endif


    A(L::FBO_RendBufSize, GL_MAX_RENDERBUFFER_SIZE),

    A(L::Tex_Size2D, GL_MAX_TEXTURE_SIZE),
    A(L::Tex_SizeCube, GL_MAX_CUBE_MAP_TEXTURE_SIZE),

    /* Extended OpenGL 3.3+ limits */
    #if GL_VERSION_VERIFY(0x300, 0x300)

    #if defined(GL_MAX_3D_TEXTURE_SIZE)
    A(L::Tex_Size3D,GL_MAX_3D_TEXTURE_SIZE),
    #endif
    #if defined(GL_MAX_ARRAY_TEXTURE_LAYERS)
    A(L::Tex_SizeArray,GL_MAX_ARRAY_TEXTURE_LAYERS),
    #endif
    #if defined(GL_MAX_TEXTURE_BUFFER_SIZE)
    A(L::Tex_SizeBuf,GL_MAX_TEXTURE_BUFFER_SIZE),
    #endif
    A(L::Tex_LODBias,GL_MAX_TEXTURE_LOD_BIAS),

    #if defined(GL_ARB_sparse_texture)
    A(L::Tex_SizeSparse2D,GL_MAX_SPARSE_TEXTURE_SIZE_ARB),
    A(L::Tex_SizeSparse3D,GL_MAX_SPARSE_3D_TEXTURE_SIZE_ARB),
    A(L::Tex_SizeSparseArray,GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS_ARB),
    #endif


    A(L::VAO_ElementIndex,GL_MAX_ELEMENT_INDEX),
    A(L::VAO_ElementIndices,GL_MAX_ELEMENTS_INDICES),
    A(L::VAO_ElementVerts,GL_MAX_ELEMENTS_VERTICES),

    #if GL_VERSION_VERIFY(0x330, 0x320)
    A(L::Vertex_AttrRelativeOff,GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET),
    A(L::Vertex_AttrStride,GL_MAX_VERTEX_ATTRIB_STRIDE),
    A(L::Vertex_AttrBindings,GL_MAX_VERTEX_ATTRIB_BINDINGS),
    #endif

    #if defined(GL_MAX_VERTEX_STREAMS)
    A(L::Vertex_Streams,GL_MAX_VERTEX_STREAMS),
    #endif

    #if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
    A(L::Vertex_ClipDists,GL_MAX_CLIP_DISTANCES),
    A(L::Vertex_CullDists,GL_MAX_CULL_DISTANCES),
    A(L::Vertex_CombClipCullDists,GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES),
    #endif

    #if GL_VERSION_VERIFY(0x330, 0x320)
    A(L::Geom_InComps,GL_MAX_GEOMETRY_INPUT_COMPONENTS),
    A(L::Geom_OutComps,GL_MAX_GEOMETRY_OUTPUT_COMPONENTS),
    A(L::Geom_OutVerts,GL_MAX_GEOMETRY_OUTPUT_VERTICES),
    #endif

    #if defined(GL_MAX_TESS_PATCH_COMPONENTS)
    A(L::Tess_Patches,GL_MAX_TESS_PATCH_COMPONENTS),
    #endif

    A(L::FBO_DrawBufs,GL_MAX_DRAW_BUFFERS),
    A(L::FBO_RendBufSize,GL_MAX_RENDERBUFFER_SIZE),
    A(L::FBO_ColorAtt,GL_MAX_COLOR_ATTACHMENTS),

    #if GL_VERSION_VERIFY(0x330, 0x320)
    A(L::FBO_Width,GL_MAX_FRAMEBUFFER_WIDTH),
    A(L::FBO_Height,GL_MAX_FRAMEBUFFER_HEIGHT),
    A(L::FBO_Layers,GL_MAX_FRAMEBUFFER_LAYERS),
    A(L::FBO_Samples,GL_MAX_FRAMEBUFFER_SAMPLES),
    #endif

    #if defined(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS)
    A(L::XF_InterleavComps,GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS),
    A(L::XF_SeparateComps,GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS),
    A(L::XF_SeparateAttrs,GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS),
    #endif

    #if GL_VERSION_VERIFY(0x330, 0x320)
    A(L::Dbg_LabelLen,GL_MAX_LABEL_LENGTH),
    A(L::Dbg_MessageLen,GL_MAX_DEBUG_MESSAGE_LENGTH),
    A(L::Dbg_LoggedMessages,GL_MAX_DEBUG_LOGGED_MESSAGES),
    #endif

//    #if defined(GL_MAX_SHADER_COMPILER_THREADS_ARB)
//    A(L::Compile_Threads,GL_MAX_SHADER_COMPILER_THREADS_ARB),
//    #endif
    #if defined(GL_MAX_SHADER_COMPILER_THREADS_KHR)
    A(L::Compile_Threads,GL_MAX_SHADER_COMPILER_THREADS_KHR),
    #endif

    #if GL_VERSION_VERIFY(0x330, 0x320)
    A(L::UniformLocs,GL_MAX_UNIFORM_LOCATIONS),
    #endif

    A(L::UBO_Size,GL_MAX_UNIFORM_BLOCK_SIZE),
    #if defined(GL_MAX_SHADER_STORAGE_BLOCK_SIZE)
    A(L::SSBO_Size,GL_MAX_SHADER_STORAGE_BLOCK_SIZE),
    #endif
    #if defined(GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE)
    A(L::AtomicBuf_Size,GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE),
    #endif

    A(L::UBO_Bindings,GL_MAX_UNIFORM_BUFFER_BINDINGS),
    #if defined(GL_MAX_SHADER_STORAGE_BLOCK_SIZE)
    A(L::SSBO_Bindings,GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS),
    #endif
    #if defined(GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE)
    A(L::AtomicBuf_Bindings,GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS),
    #endif

    #endif // GL_VERSION_VERIFY(0x300, 0x300)
};

static Map<u32, limit_name_t> limit_map_2d =  {
    A(L::View_Dimensions, GL_MAX_VIEWPORT_DIMS),
};

using DBG = CGL_Shared_Debug;

i32 CGL_Shared_Limits::Max(u32 v)
{
    GLuint pname = limit_map[v].i;

    if(pname == 0)
        return 0;

    return DBG::GetInteger(pname);
}

cstring CGL_Shared_Limits::MaxName(u32 v)
{
    auto const& attr = limit_map[v];
    if(attr.i != 0)
        return attr.name;
    return nullptr;
}

_cbasic_size_2d<i32> CGL_Shared_Limits::MaxSize(u32 v)
{
    _cbasic_size_2d<i32> size;

    GLuint pname = limit_map_2d[v].i;

    if(pname != 0)
        DBG::GetIntegerv(pname, &size.w);

    return size;
}

}
}
