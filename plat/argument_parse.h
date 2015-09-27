#ifndef ARGUMENT_PARSE
#define ARGUMENT_PARSE

#include "plat/platform_detect.h"

#include "coffee_macros.h"
#include "coffee_types.h"

#include <vector>

#define COFFEE_ARG_SWITCH '-'

using namespace Coffee;

namespace Coffee{

static cstring _switch_short(cstring in)
{
    if(strlen(in)<2)
        return in;
    if(in[0]!=COFFEE_ARG_SWITCH)
        return in;
    return &in[1];
}
static cstring _switch_long(cstring in)
{
    if(strlen(in)<3)
        return in;
    return &in[2];
}
static bool _cmp_short_switch(cstring in, cstring sw)
{
    return strcmp(_switch_short(in),sw)==0;
}
static bool _cmp_long_switch(cstring in, cstring sw)
{
    return strcmp(_switch_long(in),sw)==0;
}
static bool coffee_args_check_switch(int argc, cstring_w* argv, cstring sw)
{
    for(int i=0;i<argc;i++)
        if(_cmp_short_switch(argv[i],sw)||_cmp_long_switch(argv[i],sw))
            return true;
    return false;
}
static cstring coffee_args_get_arg(int argc, cstring_w* argv, cstring sw)
{
    for(int i=0;i<argc-1;i++)
        if(_cmp_short_switch(argv[i],sw)||_cmp_long_switch(argv[i],sw))
            return argv[i+1];
    return nullptr;
}

#if defined(COFFEE_LINUX)

#include <stdlib.h>

static cstring coffee_executable_name()
{
    return realpath("/proc/self/exe",nullptr);
}

#elif defined(COFFEE_WINDOWS)

#include <Windows.h>
#include <WinUser.h>

#define MAX_FILEPATH_SIZE 255

static cstring coffee_executable_name(cstring_w path = nullptr)
{
	if(!path)
		path = (cstring_w)malloc(MAX_FILEPATH_SIZE);

	DWORD size = GetModuleFileNameA(NULL,path,MAX_FILEPATH_SIZE);
	path[size] = '\0';
	return path;
}

#elif defined(COFFEE_APPLE)

static cstring coffee_executable_name();

#endif

}

#endif
