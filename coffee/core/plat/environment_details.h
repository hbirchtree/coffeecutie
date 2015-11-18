#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include "coffee/core/CTypes"
#include "platform_detect.h"
#include "argument_parse.h"
#include <stdlib.h>

#if defined(COFFEE_LINUX)

#include <libgen.h>
#include <unistd.h>

#elif defined(COFFEE_WINDOWS)

#include <Windows.h>
#include <direct.h>
#include <PathCch.h>

#endif

namespace Coffee{

static cstring_w coffee_get_env_variable(cstring var)
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

static cstring coffee_get_path_sep()
{
    //Assuming that Windows is the only platform using
#if defined(COFFEE_WINDOWS)
    return "\\";
#else
    return "/";
#endif
}

static cstring_w coffee_concat_plat_path(cstring_w target, cstring v2)
{
    szptr len = ((target) ? strlen(target)+1 : 0)+strlen(v2)+1;
    cstring_w p = (cstring_w)c_realloc(target,len);
    if(target) //In the case where we start out with an empty buffer
        strcat(p,coffee_get_path_sep());
    else
        p[0] = '\0';
    return strcat(p,v2);
}

static cstring_w coffee_get_user_home_dir()
{
#if defined(COFFEE_LINUX)
    return coffee_get_env_variable("HOME");
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

static cstring_w coffee_get_userdata_dir(cstring orgname, cstring appname)
{
    cstring_w base = coffee_concat_plat_path(
                nullptr,
                coffee_get_user_home_dir());
#if defined(COFFEE_LINUX)
    base = coffee_concat_plat_path(
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
    base = coffee_concat_plat_path(
                base,
                orgname);
    return coffee_concat_plat_path(base,appname);
}

static cstring_w coffee_get_application_dir()
{
#if defined(COFFEE_LINUX)
    return dirname(coffee_executable_name());
#elif defined(COFFEE_WINDOWS)
	cstring_w path = coffee_executable_name();
	cwstring_w pathw = c_wideconvert(path);
	if (PathCchRemoveFileSpec(pathw, strlen(path) + 1) != S_OK)
		return nullptr;

#endif
}

inline static cstring_w coffee_get_current_dir()
{
#if defined(COFFEE_LINUX)
    cstring_w cwd = (cstring_w)c_alloc(COFFEE_MAX_FILEPATH_BUFFER_SIZE);
    return getcwd(cwd,COFFEE_MAX_FILEPATH_BUFFER_SIZE);
#elif defined(COFFEE_WINDOWS)
	cstring_w cwd = (cstring_w)c_alloc(COFFEE_MAX_FILEPATH_BUFFER_SIZE);
	return _getcwd(cwd, COFFEE_MAX_FILEPATH_BUFFER_SIZE);
#endif
}

} //Coffee

#endif
