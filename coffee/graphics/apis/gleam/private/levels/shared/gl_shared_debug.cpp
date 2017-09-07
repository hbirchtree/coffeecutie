#include <coffee/graphics/apis/gleam/levels/shared/gl_shared_debug.h>
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
#if !defined(COFFEE_ONLY_GLES20)
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
#if !defined(COFFEE_ONLY_GLES20)
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
#if !defined(COFFEE_ONLY_GLES20)
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
#if defined(COFFEE_GLEAM_DESKTOP)
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

#if !defined(COFFEE_GLEAM_DESKTOP)
    ver.revision = 1;
#endif

#if defined(COFFEE_ONLY_GLES20)
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
    /* TODO: GL_COMPRESSED_TEXTURE_FORMATS */
    int32 supp = GL_FALSE;
    i32 target = C_CAST<i32>(to_enum(t));
    for(int32 i=0;i<Num_Internal_Formats;i++)
        if(target == Internal_Formats[i])
        {
            supp = GL_TRUE;
            break;
        }
    return supp == GL_TRUE;
}

}
}

