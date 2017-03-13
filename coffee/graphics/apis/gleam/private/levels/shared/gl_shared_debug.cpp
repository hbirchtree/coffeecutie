#include <coffee/graphics/apis/gleam/levels/shared/gl_shared_debug.h>
#include <coffee/core/string_casting.h>

namespace Coffee{
namespace CGL{

void CGL_Shared_Debug::SetDebugMode(bool enabled)
{
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

    s_ExtensionList = std::string();
    s_ExtensionList.reserve(C_CAST<size_t>(numExtensions*20));
    for(int32 i=0;i<numExtensions;i++)
    {
        cstring str = GetStringi(GL_EXTENSIONS,C_CAST<uint32>(i));
        s_ExtensionList.append(str);
        if(i<numExtensions-1)
            s_ExtensionList.push_back(' ');
    }
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
#endif

    /* In some cases, we run on drivers that are old or crappy.
         * We still want to know what the hardware supports, though. */
    do
    {
        CString str = GetString(GL_VERSION);

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

bool CGL_Shared_Debug::CompressedFormatSupport(Texture, PixelFormat t)
{
    /* TODO: GL_COMPRESSED_TEXTURE_FORMATS */
    int32 supp = GL_FALSE;
    CGenum target = to_enum(t);
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

