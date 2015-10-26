#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include "../coffee_types.h"
#include "platform_detect.h"
#include "argument_parse.h"
#include <stdlib.h>

#if defined(COFFEE_LINUX)

#include <libgen.h>
#include <unistd.h>

#endif

namespace Coffee{

static cstring_w coffee_get_env_variable(cstring var)
{
    return getenv(var);
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
    cstring_w p = (cstring_w)realloc(target,len);
    if(target) //In the case where we start out with an empty buffer
        strcat(p,coffee_get_path_sep());
    else
        p[0] = '\0';
    return strcat(p,v2);
}

static cstring_w coffee_get_userdata_dir(cstring orgname, cstring appname)
{
    cstring_w base = coffee_concat_plat_path(
                nullptr,
                coffee_get_env_variable("HOME"));
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
#else
    return "";
#endif
}

inline static cstring_w coffee_get_current_dir()
{
#if defined(COFFEE_LINUX)
    cstring_w cwd = (cstring_w)malloc(COFFEE_MAX_FILEPATH_BUFFER_SIZE);
    return getcwd(cwd,COFFEE_MAX_FILEPATH_BUFFER_SIZE);
#else
    return "";
#endif
}

} //Coffee

#endif
