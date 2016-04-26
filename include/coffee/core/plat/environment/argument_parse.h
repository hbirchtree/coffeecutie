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

    STATICINLINE bool Check(int argc, cstring* argv, cstring sw, int* pos)
    {
        for(int i=0;i<argc;i++)
            if(_cmp_short_switch(argv[i],sw[0])||_cmp_long_switch(argv[i],sw))
            {
                if(pos)
                    *pos = i;
                return true;
            }
        return false;
    }
    STATICINLINE cstring Get(int argc, cstring* argv, cstring sw, int* pos)
    {
        for(int i=1;i<argc-1;i++)
            if(_cmp_short_switch(argv[i],sw[0])||_cmp_long_switch(argv[i],sw))
            {
                if(pos)
                    *pos = i;
                return argv[i+1];
            }
        return nullptr;
    }

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

    FORCEDINLINE bool registerArgument(ArgType l, cstring lname, cstring sname = nullptr)
    {
        if(l != Switch && l != Argument)
            return false;

        arg_variant var = {};
        var.type = l;

        arguments.push_back(var);
        long_args[lname] = &arguments.back();
        if(sname)
            short_args[lname] = &arguments.back();

        return true;
    }

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
                      ArgType type, cstring sw, arg_variant& var)
    {
        if(type == Switch)
        {
            int idx = -1;
            var.switched = ArgParse::Check(argc,argv,sw,&idx);
            if(idx>=0)
                _indices.insert(idx);
        }else{
            int idx = -1;
            var.data = ArgParse::Get(argc,argv,sw,&idx);
            if(idx>=0)
            {
                _indices.insert(idx);
				_indices.insert(idx+1);
            }
        }
    }

public:

    FORCEDINLINE void parseArguments(int argc, cstring_w* _argv)
    {
        cstring* argv = (cstring*)_argv;

        argv = &argv[1];
        argc--;

        Set<int> _indices;

        for(std::pair<CString,arg_variant*> const& arg : short_args)
            get_argument(argc,argv,_indices,arg.second->type,arg.first.data(),*arg.second);

        for(std::pair<CString,arg_variant*> const& arg : long_args)
            get_argument(argc,argv,_indices,arg.second->type,arg.first.data(),*arg.second);

        for(int i=0;i<argc;i++)
            if(!_indices.count(i))
                positional.push_back(argv[i]);
    }

    FORCEDINLINE Map<CString,cstring> getArgumentOptions()
    {
        Map<CString,cstring> opts;
        for(std::pair<CString,arg_variant*> const& opt : long_args)
        {
            if(opt.second->type == Argument)
                opts[opt.first] = opt.second->data;
        }
        return opts;
    }
    FORCEDINLINE Map<CString,bool> getSwitchOptions()
    {
        Map<CString,bool> opts;
        for(std::pair<CString,arg_variant*> const& opt : long_args)
        {
            if(opt.second->type == Switch)
                opts[opt.first] = opt.second->switched;
        }
        return opts;
    }

    FORCEDINLINE Vector<cstring> const& getPositionalArguments()
    {
        return positional;
    }
};

}

#endif
