#include <coffee/core/plat/environment/environment_details.h>

#include <coffee/core/plat/platform_detect.h>
#include <coffee/core/plat/environment/argument_parse.h>

#if defined(COFFEE_ANDROID)
//#include "android/jni_props.h"
#elif defined(COFFEE_LINUX)

#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>

#elif defined(COFFEE_WINDOWS)

#include <Windows.h>
#include <direct.h>
#include <PathCch.h>

#endif

namespace Coffee{

bool EnvInterface::TermScreen::UsingAlternateBuffer = false;

#ifdef OLD_ENVIMPL
namespace Env{
bool TermScreen::UsingAlternateBuffer = false;

cstring_w GetVar(cstring var)
{
#if defined(COFFEE_LINUX)
    return getenv(var);
#elif defined(COFFEE_WINDOWS)
    int32 envSize = GetEnvironmentVariable(var,nullptr,0);
    if (envSize == 0)
        return nullptr;
    cstring_w env_var = (cstring_w)c_alloc(envSize+1);
    GetEnvironmentVariable(var,env_var,envSize+1);
    return env_var;
#endif
}

bool SetVar(cstring var, cstring value)
{
#if defined(COFFEE_ANDROID)
    return false;
#elif defined(COFFEE_UNIXPLAT)
    return setenv(var,value,0)==0;
#elif defined(COFFEE_WINDOWS)
    return false;
#endif
}

bool UnsetVar(cstring var)
{
#if defined(COFFEE_ANDROID)
    return false;
#elif defined(COFFEE_UNIXPLAT)
    return unsetenv(var)==0;
#elif defined(COFFEE_WINDOWS)
    return false;
#endif
}

bool ClearEnv()
{
#if defined(COFFEE_ANDROID)
    return false;
#elif defined(COFFEE_UNIXPLAT)
    return clearenv()==0;
#elif defined(COFFEE_WINDOWS)
    return false;
#endif
}

cstring_w ConcatPath(cstring_w target, cstring v2)
{
    szptr len = ((target) ? CStrLen(target)+1 : 0)+CStrLen(v2)+1;
    cstring_w p = (cstring_w)CRealloc(target,len);
    if(target) //In the case where we start out with an empty buffer
        CStrCat(p,GetPathSep());
    else
        p[0] = '\0';
    return CStrCat(p,v2);
}

cstring_w GetUserHome()
{
#if defined(COFFEE_ANDROID)
    return nullptr;
#elif defined(COFFEE_LINUX)
    return GetVar("HOME");
#elif defined(COFFEE_WINDOWS)
    cstring_w homedrive = coffee_get_env_variable("HOMEDRIVE");
    cstring_w homepath = coffee_get_env_variable("HOMEPATH");
    cstring_w homedir = coffee_concat_plat_path(homedrive,homepath);
    c_free(homepath);
    return homedir;
#else
    return nullptr;
#endif
}

cstring GetPathSep()
{
    //Assuming that Windows is the only platform using
#if defined(COFFEE_WINDOWS)
    return "\\";
#else
    return "/";
#endif
}

cstring_w GetUserData(cstring orgname, cstring appname)
{
    cstring_w base = ConcatPath(
                nullptr,
                GetUserHome());
#if defined(COFFEE_ANDROID)
    /*
     * We define Android before Linux as an override.
     * Android identifies as both Android and Linux, thus we are overriding it here.
    */

    base = "";

#elif defined(COFFEE_LINUX)
    base = ConcatPath(
                base,
                ".local/share");
#elif defined(COFFEE_WINDOWS)
    base = coffee_concat_plat_path(
                base,
                "AppData\\Local");
#elif defined(COFFEE_APPLE)
    base = coffee_concat_plat_path(
                base,
                "Library/Application Support");
#endif
    base = ConcatPath(
                base,
                orgname);
    return ConcatPath(base,appname);
}

cstring_w ApplicationDir()
{
#if defined(COFFEE_ANDROID)
    return nullptr;
#elif defined(COFFEE_LINUX)
    return dirname(ExecutableName());
#elif defined(COFFEE_WINDOWS)
    cstring_w path = coffee_executable_name();
    cwstring_w pathw = c_str_wideconvert(path);
    if (PathCchRemoveFileSpec(pathw, strlen(path) + 1) != S_OK)
        return nullptr;
    c_free(path);
    path = c_str_narrowconvert(pathw);
    c_free(pathw);
    return path;
#endif
}

cstring_w CurrentDir()
{
#if defined(COFFEE_ANDROID)
    return nullptr;
#elif defined(COFFEE_LINUX)
    cstring_w cwd = (cstring_w)Alloc(COFFEE_MAX_FILEPATH_BUFFER_SIZE);
    return getcwd(cwd,COFFEE_MAX_FILEPATH_BUFFER_SIZE);
#elif defined(COFFEE_WINDOWS)
    cstring_w cwd = (cstring_w)c_alloc(COFFEE_MAX_FILEPATH_BUFFER_SIZE);
    return _getcwd(cwd, COFFEE_MAX_FILEPATH_BUFFER_SIZE);
#endif
}

}
#endif
}
