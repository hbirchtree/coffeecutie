#ifndef ARGUMENT_PARSE
#define ARGUMENT_PARSE

#define COFFEE_MAX_FILEPATH_BUFFER_SIZE 255
#define COFFEE_ARG_SWITCH '-'

#include "../platform_detect.h"
#include "../../coffee_macros.h"
#include "../../types/tdef/stltypes.h"
#include "../../types/tdef/integertypes.h"
#include "../memory/cmemory.h"

namespace Coffee{

struct ArgumentCollection;

struct ArgParse
{
    friend struct ArgumentCollection;

protected:
    STATICINLINE char _switch_short(cstring in)
    {
        /* If len is less than 2, it's not what we look for */
        if(StrLen(in)<2)
            return 0;
        /* If first character is not "-" or "/", return with doomed ptr */
        if(in[0]!=COFFEE_ARG_SWITCH)
            return 0;
        /* If all is good, return ptr to switch part */
        return in[1];
    }
    STATICINLINE cstring _switch_long(cstring in)
    {
        if(StrLen(in)<3||in[0]!=COFFEE_ARG_SWITCH||in[1]!=COFFEE_ARG_SWITCH)
            return in;
        return &in[2];
    }
    STATICINLINE bool _cmp_short_switch(cstring in, char sw)
    {
        /* If input is large, we probably don't want to parse it with this */
        return _switch_short(in)==sw;
    }
    STATICINLINE bool _cmp_long_switch(cstring in, cstring sw)
    {
        return StrCmp(_switch_long(in),sw);
    }

    static bool Check(int argc, cstring* argv, cstring sw, int* pos);
    static cstring Get(int argc, cstring* argv, cstring sw, int* pos);

public:
    STATICINLINE bool Check(int argc, cstring* argv, cstring sw)
    {
        return Check(argc,argv,sw,nullptr);
    }
    STATICINLINE cstring Get(int argc, cstring* argv, cstring sw)
    {
        return Get(argc,argv,sw,nullptr);
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

struct ArgumentCollection
{
public:
    enum ArgType
    {
        Switch,
        Argument,
    };

    bool registerArgument(ArgType l, cstring lname,
                          cstring sname = nullptr,
                          cstring information = nullptr);

private:
    struct arg_variant
    {
        ArgType type;
        union
        {
            bool switched;
            cstring data;
        };
    };

    Vector<cstring> positional;
    LinkList<arg_variant> arguments;
    Map<CString,arg_variant*> short_args;
    Map<CString,arg_variant*> long_args;

    void get_argument(int argc, cstring* argv, Set<int>& _indices,
                      ArgType type, cstring sw, arg_variant& var);

public:

    void parseArguments(int argc, cstring_w* _argv);
    Map<CString,cstring> getArgumentOptions();
    Map<CString,bool> getSwitchOptions();

    FORCEDINLINE Vector<cstring> const& getPositionalArguments()
    {
        return positional;
    }
};

}

#endif
