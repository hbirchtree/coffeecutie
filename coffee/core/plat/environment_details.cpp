#include "environment_details.h"

#include "platform_detect.h"
#include "argument_parse.h"

#if defined(COFFEE_LINUX)

#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>

#elif defined(COFFEE_WINDOWS)

#include <Windows.h>
#include <direct.h>
#include <PathCch.h>

#endif

namespace Coffee{

cstring_w env_get_variable(cstring var)
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

bool env_set_variable(cstring var, cstring value)
{
#if defined(COFFEE_UNIXPLAT)
    return setenv(var,value,0)==0;
#elif defined(COFFEE_WINDOWS)
    return false;
#endif
}

bool env_unset_variable(cstring var)
{
#if defined(COFFEE_UNIXPLAT)
    return unsetenv(var)==0;
#elif defined(COFFEE_WINDOWS)
    return false;
#endif
}

bool env_clear_all()
{
#if defined(COFFEE_UNIXPLAT)
    return clearenv()==0;
#elif defined(COFFEE_WINDOWS)
    return false;
#endif
}

cstring_w env_concatenate_path(cstring_w target, cstring v2)
{
    szptr len = ((target) ? c_strlen(target)+1 : 0)+c_strlen(v2)+1;
    cstring_w p = (cstring_w)c_realloc(target,len);
    if(target) //In the case where we start out with an empty buffer
        c_strcat(p,env_get_path_separator());
    else
        p[0] = '\0';
    return c_strcat(p,v2);
}

cstring_w env_get_user_home()
{
#if defined(COFFEE_LINUX)
    return env_get_variable("HOME");
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

cstring env_get_path_separator()
{
    //Assuming that Windows is the only platform using
#if defined(COFFEE_WINDOWS)
    return "\\";
#else
    return "/";
#endif
}

cstring_w env_get_user_data(cstring orgname, cstring appname)
{
    cstring_w base = env_concatenate_path(
                nullptr,
                env_get_user_home());
#if defined(COFFEE_LINUX)
    base = env_concatenate_path(
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
    base = env_concatenate_path(
                base,
                orgname);
    return env_concatenate_path(base,appname);
}

cstring_w env_get_application_dir()
{
#if defined(COFFEE_LINUX)
    return dirname(executable_name());
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

cstring_w env_get_current_dir()
{
#if defined(COFFEE_LINUX)
    cstring_w cwd = (cstring_w)c_alloc(COFFEE_MAX_FILEPATH_BUFFER_SIZE);
    return getcwd(cwd,COFFEE_MAX_FILEPATH_BUFFER_SIZE);
#elif defined(COFFEE_WINDOWS)
    cstring_w cwd = (cstring_w)c_alloc(COFFEE_MAX_FILEPATH_BUFFER_SIZE);
    return _getcwd(cwd, COFFEE_MAX_FILEPATH_BUFFER_SIZE);
#endif
}

}
