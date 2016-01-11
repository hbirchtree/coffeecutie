#ifndef ARGUMENT_PARSE
#define ARGUMENT_PARSE

#define COFFEE_MAX_FILEPATH_BUFFER_SIZE 255
#define COFFEE_ARG_SWITCH '-'

#include "coffee/core/plat/platform_detect.h"
#include "coffee/core/coffee_macros.h"
#include <coffee/core/CTypes>

namespace Coffee{

extern cstring_w executable_name(cstring_w n = nullptr);

static cstring _switch_short(cstring in)
{
    if(CStrLen(in)<2)
        return in;
    if(in[0]!=COFFEE_ARG_SWITCH)
        return in;
    return &in[1];
}
static cstring _switch_long(cstring in)
{
    //Very crude, indeed
    if(CStrLen(in)<3||in[0]!=COFFEE_ARG_SWITCH||in[1]!=COFFEE_ARG_SWITCH)
        return in;
    return &in[2];
}
static bool _cmp_short_switch(cstring in, cstring sw)
{
    return CStrCmp(_switch_short(in),sw)==0;
}
static bool _cmp_long_switch(cstring in, cstring sw)
{
    return CStrCmp(_switch_long(in),sw)==0;
}
inline static bool CheckSwitch(
        int argc, cstring_w* argv, cstring sw)
{
    for(int i=0;i<argc;i++)
        if(_cmp_short_switch(argv[i],sw)||_cmp_long_switch(argv[i],sw))
            return true;
    return false;
}
inline static cstring GetArgument(
        int argc, cstring_w* argv, cstring sw)
{
    for(int i=0;i<argc-1;i++)
        if(_cmp_short_switch(argv[i],sw)||_cmp_long_switch(argv[i],sw))
            return argv[i+1];
    return nullptr;
}

/*!
 * \brief Acquire all arguments listed by in_argv and write them to out_argv, pulling data from argv. Relatively high computational complexity.
 * \param argc Number of input arguments
 * \param argv Source for arguments
 * \param get_argc Number of arguments to acquire, needs to be same size as in_argv and out_argv
 * \param in_argv Contains null-terminated strings with names of switches
 * \param out_argv
 */
inline void coffee_args_get_all(
        int argc, cstring_w* argv,
        int get_argc, cstring const* in_argv,
        cstring* out_argv)
{
    /*TODO: Write a better implementation that maps from argv to in_argv*/
    for(int i=0;i<get_argc;i++)
    {
        out_argv[i] = GetArgument(argc,argv,in_argv[i]);
    }
}

}

#endif
