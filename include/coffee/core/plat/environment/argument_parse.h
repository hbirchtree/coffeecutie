#ifndef ARGUMENT_PARSE
#define ARGUMENT_PARSE

#define COFFEE_MAX_FILEPATH_BUFFER_SIZE 255
#define COFFEE_ARG_SWITCH '-'

#include "../platform_detect.h"
#include "../../coffee_macros.h"
#include "../../types/basetypes.h"

namespace Coffee{

struct ArgParse
{
protected:
    STATICINLINE cstring _switch_short(cstring in)
    {
        if(StrLen(in)<2)
            return in;
        if(in[0]!=COFFEE_ARG_SWITCH)
            return in;
        return &in[1];
    }
    STATICINLINE cstring _switch_long(cstring in)
    {
        if(StrLen(in)<3||in[0]!=COFFEE_ARG_SWITCH||in[1]!=COFFEE_ARG_SWITCH)
            return in;
        return &in[2];
    }
    STATICINLINE bool _cmp_short_switch(cstring in, cstring sw)
    {
        return StrCmp(_switch_short(in),sw)==0;
    }
    STATICINLINE bool _cmp_long_switch(cstring in, cstring sw)
    {
        return StrCmp(_switch_long(in),sw)==0;
    }

public:
    STATICINLINE bool Check(int argc, cstring* argv, cstring sw)
    {
        for(int i=0;i<argc;i++)
            if(_cmp_short_switch(argv[i],sw)||_cmp_long_switch(argv[i],sw))
                return true;
        return false;
    }
    STATICINLINE cstring Get(int argc, cstring* argv, cstring sw)
    {
        for(int i=1;i<argc-1;i++)
            if(_cmp_short_switch(argv[i],sw)||_cmp_long_switch(argv[i],sw))
                return argv[i+1];
        return nullptr;
    }

    STATICINLINE bool Check(int argc, cstring_w* argv, cstring sw)
    {
        return Check(argc,(cstring*)argv,sw);
    }
    STATICINLINE cstring Get(int argc, cstring_w* argv, cstring sw)
    {
        return Get(argc,(cstring*)argv,sw);
    }
};

}

#endif
