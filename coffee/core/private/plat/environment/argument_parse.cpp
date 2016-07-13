#include <coffee/core/plat/environment/argument_parse.h>

namespace Coffee{

bool ArgumentCollection::registerArgument(ArgumentCollection::ArgType l, cstring lname, cstring sname, cstring information)
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

void ArgumentCollection::get_argument(int argc, cstring *argv, Set<int> &_indices, ArgumentCollection::ArgType type, cstring sw, ArgumentCollection::arg_variant &var)
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

void ArgumentCollection::parseArguments(int argc, cstring_w *_argv)
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

Map<CString, cstring> ArgumentCollection::getArgumentOptions()
{
    Map<CString,cstring> opts;
    for(std::pair<CString,arg_variant*> const& opt : long_args)
    {
        if(opt.second->type == Argument)
            opts[opt.first] = opt.second->data;
    }
    return opts;
}

Map<CString, bool> ArgumentCollection::getSwitchOptions()
{
    Map<CString,bool> opts;
    for(std::pair<CString,arg_variant*> const& opt : long_args)
    {
        if(opt.second->type == Switch)
            opts[opt.first] = opt.second->switched;
    }
    return opts;
}

bool ArgParse::Check(int argc, cstring *argv, cstring sw, int *pos)
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

cstring ArgParse::Get(int argc, cstring *argv, cstring sw, int *pos)
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

}
