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
    using Character = char;
    using String = cstring;
    using String_w = cstring_w;

    friend struct ArgumentCollection;

protected:
    STATICINLINE String _switch_short(String in)
    {
        /* If len is less than 2, it's not what we look for */
        if(StrLen(in)<2)
            return nullptr;
        /* If first character is not "-" or "/", return with doomed ptr */
        if(in[0]!=COFFEE_ARG_SWITCH && in[1]!=COFFEE_ARG_SWITCH)
            return nullptr;
        /* If all is good, return ptr to switch part */
        return &in[1];
    }
    STATICINLINE String _switch_long(String in)
    {
        if(StrLen(in)<3||in[0]!=COFFEE_ARG_SWITCH||in[1]!=COFFEE_ARG_SWITCH)
            return in;
        return &in[2];
    }
    STATICINLINE bool _cmp_short_switch(String in, Character sw)
    {
        /* If input is large, we probably don't want to parse it with this */
        cstring opt = _switch_short(in);
        for(szptr i=0;i<StrLen(opt);i++)
            if(opt[i] == sw)
                return true;
        return false;
    }
    STATICINLINE bool _cmp_long_switch(String in, String sw)
    {
        return StrCmp(_switch_long(in),sw);
    }

    static bool Check(int argc, String* argv, String sw_l, Character sw_s, int* pos);
    static cstring Get(int argc, String* argv, String sw, Character sw_s, int* pos);

public:
    STATICINLINE bool Check(int argc, String* argv, String sw)
    {
        return Check(argc,argv,sw,0,nullptr);
    }
    STATICINLINE String Get(int argc, String* argv, String sw)
    {
        return Get(argc,argv,sw,0,nullptr);
    }

    STATICINLINE bool Check(int argc, String_w* argv, String sw)
    {
        return Check(argc,(String*)argv,sw);
    }
    STATICINLINE String Get(int argc, String_w* argv, String sw)
    {
        return Get(argc,(String*)argv,sw);
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
        cstring information;
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
                      ArgType type, cstring sw, cstring sw_s, arg_variant& var);

public:

    void parseArguments(int argc, cstring_w* _argv);
    Map<CString,cstring> getArgumentOptions();
    Map<CString,bool> getSwitchOptions();

    CString helpMessage() const;

    FORCEDINLINE Vector<cstring> const& getPositionalArguments()
    {
        return positional;
    }
};

}

#endif
